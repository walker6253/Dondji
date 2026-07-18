#include <stdint.h>
#include <stdbool.h>

// ─── U8G2 capture ───
void u8x8_capture_write_pbm_buffer(void *a, void *b) { (void)a;(void)b; }
void u8x8_capture_write_pbm_pre(void *a, void *b) { (void)a;(void)b; }
void u8x8_capture_write_xbm_buffer(void *a, void *b) { (void)a;(void)b; }
void u8x8_capture_write_xbm_pre(void *a, void *b) { (void)a;(void)b; }
int u8x8_capture_get_pixel_1(void *a, int x, int y) { (void)a;(void)x;(void)y; return 0; }
int u8x8_capture_get_pixel_2(void *a, int x, int y) { (void)a;(void)x;(void)y; return 0; }

// ─── U8G2 draw ───
void u8g2_DrawCircle(void *a, int x, int y, int r, int o) { (void)a;(void)x;(void)y;(void)r;(void)o; }
void u8g2_DrawDisc(void *a, int x, int y, int r, int o) { (void)a;(void)x;(void)y;(void)r;(void)o; }
int u8g2_GetKerning(void *a, int e, int c) { (void)a;(void)e;(void)c; return 0; }
int u8g2_GetKerningByTable(void *a, const void *t) { (void)a;(void)t; return 0; }

// ─── Linker script symbols (used by UI_MENU_GetMemPercents) ───
// In ARM linker script these are addresses. Provide dummy variables.
char _eflash_used = 0;
char _sdata = 0;
char _ebss = 0;

// ─── System ───
void NVIC_SystemReset(void) {}

// ─── printf.c needs _putchar ───
void _putchar(char c) { (void)c; }

// ─── FM Radio globals ───
bool gFM_AutoScan = false;
int gFM_ChannelPosition = 0;
int gFM_Channels[48] = {0};
int gFM_RestoreCountdown_10ms = 0;
int gFM_ScanState = 0;
int gFmPlayCountdown_10ms = 0;
int gFmRadioCountdown_500ms = 0;
int gFmRadioMode = 0;

// ─── Voice globals ───
int gAnotherVoiceID = 0;
int gBeepToPlay = 0;
int gCountdownToPlayNextVoice_10ms = 0;
bool gFlagPlayQueuedVoice = false;
int gVoiceWriteIndex = 0;

// ─── AirCopy globals ───
bool gAirCopyIsSendMode = false;
int gAircopyState = 0;
int g_FSK_Buffer[64] = {0};

// ─── Action ───
void ACTION_FlashLight(void) {}
void ACTION_RegaAlarm(void) {}
void ACTION_RegaTest(void) {}

// ─── AirCopy ───
void AIRCOPY_ProcessKeys(int a, bool b, bool c) { (void)a;(void)b;(void)c; }
void AIRCOPY_SendMessage(void) {}
void AIRCOPY_StorePacket(void) {}

// ─── Toolbox ───
void APP_RunToolboxMenu(void) {}

// ─── BK1080 FM chip ───
int BK1080_GetFreqHiLimit(void) { return 108000; }
int BK1080_GetFreqLoLimit(void) { return 76000; }
int BK1080_GetFrequencyDeviation(void) { return 0; }
void BK1080_Init0(void) {}
void BK1080_Init(void) {}
void BK1080_WriteRegister(int a, int b) { (void)a;(void)b; }

// ─── EEPROM ───
void EEPROM_ReadBuffer(uint16_t Address, void *pBuffer, uint8_t Size) { (void)Address;(void)pBuffer;(void)Size; }
void EEPROM_WriteBuffer(uint16_t Address, const void *pBuffer) { (void)Address;(void)pBuffer; }

// ─── FM ───
void FM_ConfigureChannelState(void) {}
void FM_EraseChannels(void) {}
void FM_Play(int a) { (void)a; }
void FM_PlayAndUpdate(void) {}
void FM_ProcessKeys(int a, bool b, bool c) { (void)a;(void)b;(void)c; }
void FM_Start(void) {}
void FM_Tune(int a) { (void)a; }
void FM_TurnOff(void) {}

// ─── Flashlight ───
void FlashlightTimeSlice(void) {}

// ─── UART ───
void UART_HandleCommand(void) {}
bool UART_IsCommandAvailable(void) { return false; }

// ─── UI ───
void UI_DisplayAircopy(void) {}
void UI_DisplayFM(void) {}
void UI_DisplayREGA(void) {}
