#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <SDL2/SDL.h>

// Real firmware headers
#include "driver/st7565.h"
#include "driver/bk4819.h"
#include "driver/backlight.h"
#include "driver/system.h"
#include "driver/systick.h"
#include "driver/py25q16.h"
#include "board.h"
#include "settings.h"
#include "misc.h"
#include "radio.h"
#include "frequencies.h"
#include "functions.h"
#include "helper/battery.h"
#include "helper/boot.h"
#include "app/app.h"
#include "ui/ui.h"
#include "ui/welcome.h"
#include "ui/menu.h"
#include "app/dtmf.h"
#include "app/mdc1200.h"
#include "audio.h"

// Simulator-specific: callback for rendering gFrameBuffer to SDL2
typedef void (*sim_render_cb_t)(const uint8_t status[128], const uint8_t frame[8][128]);
extern sim_render_cb_t sim_render_callback;

// Simulator keyboard injection
extern void sim_keyboard_set_key(KEY_Code_t key, bool pressed);

// SPI flash file path
extern const char *sim_flash_file;

// External globals
extern volatile uint8_t boot_counter_10ms;
extern volatile bool gNextTimeslice;
extern volatile bool gNextTimeslice_500ms;
extern volatile bool gNextTimeslice40ms;
extern uint16_t gBatteryCurrentVoltage;
extern uint16_t gBatteryCurrent;
extern uint16_t gBatteryVoltages[4];
extern uint8_t gBatteryDisplayLevel;
extern bool gChargingWithTypeC;
extern uint8_t gReducedService;
extern uint8_t gUpdateStatus;
extern uint8_t gMenuListCount;
extern char gDTMF_String[15];
extern uint8_t gDW, gCB;
extern EEPROM_Config_t gEeprom;
extern const t_menu_item MenuList[];

extern void SETTINGS_InitCNFont(void);
extern void MDC1200_init(void);
extern void AM_fix_init(void);

// SDL state
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;
static uint32_t screen_pixels[128 * 64];
static int pixel_scale = 5;
static Uint32 last_tick = 0;
static uint32_t tick_acc_10ms = 0;
static uint32_t tick_acc_500ms = 0;
static uint32_t tick_acc_40ms = 0;
static uint32_t frame_count = 0;
static Uint32 fps_timer = 0;
static bool window_focused = false;

static void sim_render(const uint8_t status[128], const uint8_t frame[8][128])
{
    for (int y = 0; y < 64; y++) {
        int page = y / 8;
        int bit = y % 8;
        const uint8_t *src = (y < 8) ? status : frame[page - 1];
        for (int x = 0; x < 128; x++) {
            screen_pixels[y * 128 + x] = (src[x] & (1 << bit)) ? 0xFF000000 : 0xFFFFFFFF;
        }
    }
}

static KEY_Code_t sdl_to_radio(SDL_Keycode sym)
{
    switch (sym) {
        case SDLK_0: return KEY_0;
        case SDLK_1: return KEY_1;
        case SDLK_2: return KEY_2;
        case SDLK_3: return KEY_3;
        case SDLK_4: return KEY_4;
        case SDLK_5: return KEY_5;
        case SDLK_6: return KEY_6;
        case SDLK_7: return KEY_7;
        case SDLK_8: return KEY_8;
        case SDLK_9: return KEY_9;
        case SDLK_m: case SDLK_RETURN: return KEY_MENU;
        case SDLK_UP: return KEY_UP;
        case SDLK_DOWN: return KEY_DOWN;
        case SDLK_ESCAPE: case SDLK_BACKSPACE: return KEY_EXIT;
        case SDLK_ASTERISK: case SDLK_KP_MULTIPLY: return KEY_STAR;
        case SDLK_f: return KEY_F;
        case SDLK_HASH: return KEY_F;
        case SDLK_p: return KEY_PTT;
        default: return KEY_INVALID;
    }
}

int main(int argc, char *argv[])
{
    printf("=== Dondji Simulator ===\n");
    printf("Keys: 0-9, M/Enter=Menu, Arrows=Up/Down, Esc=Exit, *=Star, F=Function\n");
    printf("+/- Zoom, Q Quit\n");
    printf("DEBUG: Click on the SDL window to give it keyboard focus!\n\n");

    const char *home = getenv("HOME");
    if (home) {
        static char flash_path[1024];
        snprintf(flash_path, sizeof(flash_path), "%s/.dondji_sim_flash.bin", home);
        sim_flash_file = flash_path;
    }

    // ── Firmware init ──
    SYSTICK_Init();
    BOARD_Init();
    boot_counter_10ms = 250;
    memset(gDTMF_String, '-', sizeof(gDTMF_String));
    gDTMF_String[sizeof(gDTMF_String) - 1] = 0;
    BK4819_Init();
    BOARD_ADC_GetBatteryInfo(&gBatteryCurrentVoltage, &gBatteryCurrent);
    SETTINGS_InitEEPROM();
    MDC1200_init();
    gDW = gEeprom.DUAL_WATCH;
    gCB = gEeprom.CROSS_BAND_RX_TX;
    SETTINGS_WriteBuildOptions();
    SETTINGS_LoadCalibration();
    SETTINGS_InitCNFont();
    RADIO_ConfigureChannel(0, VFO_CONFIGURE_RELOAD);
    RADIO_ConfigureChannel(1, VFO_CONFIGURE_RELOAD);
    RADIO_SelectVfos();
    RADIO_SetupRegisters(true);
    for (unsigned i = 0; i < ARRAY_SIZE(gBatteryVoltages); i++)
        BOARD_ADC_GetBatteryInfo(&gBatteryVoltages[i], &gBatteryCurrent);
    BATTERY_GetReadings(false);
    AM_fix_init();
    gMenuListCount = 0;
    while (MenuList[gMenuListCount].name[0] != '\0') gMenuListCount++;
    UI_DisplayWelcome();
    BACKLIGHT_TurnOn();
    while (boot_counter_10ms > 0) {
        usleep(10000); boot_counter_10ms--;
    }
    BOOT_ProcessMode(0);
    gUpdateStatus = true;

    // ── SDL2 init ──
    SDL_SetHint(SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES, "0");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError()); return 1;
    }
    window = SDL_CreateWindow("Dondji Simulator - Click here to focus!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        128*pixel_scale, 64*pixel_scale,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS);
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError()); return 1;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING, 128, 64);
    memset(screen_pixels, 0xFF, sizeof(screen_pixels));

    sim_render_callback = sim_render;

    // Force window to front
    SDL_RaiseWindow(window);
    SDL_ShowWindow(window);

    printf("Simulator running. Q to quit. DEBUG: window created, waiting for events...\n");
    bool running = true;
    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_WINDOWEVENT:
                if (ev.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                    window_focused = true;
                    printf("DEBUG: Window FOCUS GAINED\n");
                } else if (ev.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                    window_focused = false;
                    printf("DEBUG: Window FOCUS LOST\n");
                }
                break;
            case SDL_KEYDOWN:
                printf("DEBUG: KEYDOWN sym=0x%x scancode=%d\n",
                    ev.key.keysym.sym, ev.key.keysym.scancode);
                if (ev.key.keysym.sym == SDLK_q) running = false;
                else if (ev.key.keysym.sym == SDLK_EQUALS || ev.key.keysym.sym == SDLK_PLUS) {
                    if (pixel_scale < 12) {
                        pixel_scale++;
                        SDL_SetWindowSize(window, 128*pixel_scale, 64*pixel_scale);
                    }
                } else if (ev.key.keysym.sym == SDLK_MINUS) {
                    if (pixel_scale > 2) {
                        pixel_scale--;
                        SDL_SetWindowSize(window, 128*pixel_scale, 64*pixel_scale);
                    }
                } else if (ev.key.keysym.sym == SDLK_s) {
                    // Screenshot
                    SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(
                        screen_pixels, 128, 64, 32, 128*4,
                        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
                    if (surf) {
                        SDL_SaveBMP(surf, "screenshot.bmp");
                        SDL_FreeSurface(surf);
                        printf("DEBUG: Screenshot saved to screenshot.bmp\n");
                    }
                } else {
                    KEY_Code_t k = sdl_to_radio(ev.key.keysym.sym);
                    printf("DEBUG: Radio key=%d\n", k);
                    if (k != KEY_INVALID) sim_keyboard_set_key(k, true);
                }
                break;
            case SDL_KEYUP:
                printf("DEBUG: KEYUP sym=0x%x\n", ev.key.keysym.sym);
                {
                    KEY_Code_t k = sdl_to_radio(ev.key.keysym.sym);
                    if (k != KEY_INVALID) sim_keyboard_set_key(k, false);
                }
                break;
            }
        }

        // Periodically try to raise window (first 3 seconds)
        static Uint32 first_frame = 0;
        if (first_frame == 0) first_frame = SDL_GetTicks();
        if (SDL_GetTicks() - first_frame < 3000) {
            SDL_RaiseWindow(window);
        }

        // Timer simulation
        Uint32 now = SDL_GetTicks();
        if (last_tick == 0) last_tick = now;
        Uint32 elapsed = now - last_tick;
        last_tick = now;
        tick_acc_10ms += elapsed;
        tick_acc_500ms += elapsed;
        tick_acc_40ms += elapsed;
        if (tick_acc_10ms >= 10) { tick_acc_10ms -= 10; gNextTimeslice = true; }
        if (tick_acc_40ms >= 40) { tick_acc_40ms -= 40; gNextTimeslice40ms = true; }
        if (tick_acc_500ms >= 500) { tick_acc_500ms -= 500; gNextTimeslice_500ms = true; }

        frame_count++;
        if (now - fps_timer >= 1000) {
            char title[128];
            snprintf(title, sizeof(title), "Dondji Simulator - %u FPS [%s]",
                frame_count, window_focused ? "FOCUSED" : "CLICK ME");
            SDL_SetWindowTitle(window, title);
            frame_count = 0; fps_timer = now;
        }

        APP_Update();
        if (gNextTimeslice) {
            APP_TimeSlice10ms();
            gNextTimeslice = false;
            if (gNextTimeslice_500ms) {
                APP_TimeSlice500ms();
                gNextTimeslice_500ms = false;
            }
        }
        if (gNextTimeslice40ms) gNextTimeslice40ms = false;

        SDL_UpdateTexture(texture, NULL, screen_pixels, 128*sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    printf("Goodbye.\n");
    return 0;
}
