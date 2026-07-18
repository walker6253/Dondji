#include "driver/backlight.h"
uint16_t gBacklightCountdown_500ms;
uint8_t gBacklightBrightness = 100;
#ifdef ENABLE_FEAT_F4HWN
const uint8_t value[11] = {0,10,20,30,40,50,60,70,80,90,100};
#endif
#ifdef ENABLE_FEAT_F4HWN_SLEEP
uint16_t gSleepModeCountdown_500ms;
#endif
void BACKLIGHT_InitHardware() {}
void BACKLIGHT_UpdateTickless(void) {}
void BACKLIGHT_TurnOn() {}
void BACKLIGHT_TurnOff() {}
bool BACKLIGHT_IsOn() { return true; }
void BACKLIGHT_SetBrightness(uint8_t b) { gBacklightBrightness = b; }
void BACKLIGHT_Update(void) {}
uint8_t BACKLIGHT_GetBrightness(void) { return gBacklightBrightness; }
