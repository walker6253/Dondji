#include "driver/keyboard.h"
#include <stdbool.h>

KEY_Code_t gKeyReading0     = KEY_INVALID;
KEY_Code_t gKeyReading1     = KEY_INVALID;
uint16_t   gDebounceCounter = 0;
bool       gWasFKeyPressed  = false;

#ifdef ENABLE_FEAT_F4HWN_SCREENSHOT
volatile KEY_Code_t gKeyFromSerial = KEY_INVALID;
bool KEYBOARD_ProcessProtocolByte(ParseState_t *s, uint8_t b)
{ (void)s;(void)b; return false; }
#endif

static KEY_Code_t sim_pending = KEY_INVALID;
static bool sim_held = false;
static int sim_cnt = 0;

void sim_keyboard_set_key(KEY_Code_t key, bool pressed)
{
    if (pressed) { sim_pending = key; sim_held = true; sim_cnt = 0; }
    else { sim_held = false; }
}

KEY_Code_t KEYBOARD_Poll(void)
{
    if (sim_held) {
        sim_cnt++;
        if (sim_cnt == 1) return sim_pending;
        if (sim_cnt > 40 && (sim_cnt % 5) == 0) return sim_pending;
    }
    return KEY_INVALID;
}

KEY_Code_t KEYBOARD_GetKey(void) { return KEYBOARD_Poll(); }
void HideFKeyIcon(void) {}

// Workaround: channelMove() in app/main.c uses undeclared 'Key'
int Key = 0;
