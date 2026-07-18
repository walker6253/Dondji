#include "driver/bk4819.h"
#include <stdbool.h>
#include <stdint.h>

bool gRxIdleMode = true;

void BK4819_Init(void) {}
uint16_t BK4819_ReadRegister(BK4819_REGISTER_t r) { (void)r; return 0; }
void BK4819_WriteRegister(BK4819_REGISTER_t r, uint16_t d) { (void)r;(void)d; }
void BK4819_SetRegValue(RegisterSpec s, uint16_t v) { (void)s;(void)v; }
void BK4819_WriteU8(uint8_t d) { (void)d; }
void BK4819_WriteU16(uint16_t d) { (void)d; }
void BK4819_SetAGC(bool e) { (void)e; }
void BK4819_InitAGC(bool am) { (void)am; }
void BK4819_ToggleGpioOut(BK4819_GPIO_PIN_t p, bool s) { (void)p;(void)s; }
void BK4819_SetCDCSSCodeWord(uint32_t c) { (void)c; }
void BK4819_SetCTCSSFrequency(uint32_t f) { (void)f; }
void BK4819_SetTailDetection(const uint32_t f) { (void)f; }
void BK4819_EnableVox(uint16_t t1, uint16_t t0) { (void)t1;(void)t0; }
void BK4819_SetFilterBandwidth(const BK4819_FilterBandwidth_t bw, const bool w) { (void)bw;(void)w; }
void BK4819_SetupPowerAmplifier(const uint8_t bias, const uint32_t f) { (void)bias;(void)f; }
void BK4819_SetFrequency(uint32_t f) { (void)f; }
void BK4819_SetupSquelch(uint8_t a,uint8_t b,uint8_t c,uint8_t d,uint8_t e,uint8_t f) { (void)a;(void)b;(void)c;(void)d;(void)e;(void)f; }
void BK4819_SetAF(BK4819_AF_Type_t af) { (void)af; }
void BK4819_RX_TurnOn(void) {}
void BK4819_PickRXFilterPathBasedOnFrequency(uint32_t f) { (void)f; }
void BK4819_DisableScramble(void) {}
void BK4819_EnableScramble(uint8_t t) { (void)t; }
bool BK4819_CompanderEnabled(void) { return false; }
void BK4819_SetCompander(const unsigned int m) { (void)m; }
void BK4819_DisableVox(void) {}
void BK4819_DisableDTMF(void) {}
void BK4819_EnableDTMF(void) {}
void BK4819_PlayTone(uint16_t f, bool b) { (void)f;(void)b; }
void BK4819_PlaySingleTone(const unsigned int t,const unsigned int d,const unsigned int l,const bool s) { (void)t;(void)d;(void)l;(void)s; }
void BK4819_EnterTxMute(void) {}
void BK4819_ExitTxMute(void) {}
void BK4819_Sleep(void) {}
void BK4819_TurnsOffTones_TurnsOnRX(void) {}
void BK4819_ResetFSK(void) {}
void BK4819_Idle(void) {}
void BK4819_ExitBypass(void) {}
void BK4819_PrepareTransmit(void) {}
void BK4819_TxOn_Beep(void) {}
void BK4819_ExitSubAu(void) {}
void BK4819_Conditional_RX_TurnOn_and_GPIO6_Enable(void) {}
void BK4819_EnterDTMF_TX(bool b) { (void)b; }
void BK4819_ExitDTMF_TX(bool b) { (void)b; }
void BK4819_EnableTXLink(void) {}
void BK4819_PlayDTMF(char c) { (void)c; }
void BK4819_PlayDTMFString(const char *s,bool b,uint16_t a,uint16_t d,uint16_t e,uint16_t f) { (void)s;(void)b;(void)a;(void)d;(void)e;(void)f; }
void BK4819_TransmitTone(bool b, uint32_t f) { (void)b;(void)f; }
void BK4819_GenTail(uint8_t t) { (void)t; }
void BK4819_PlayCDCSSTail(void) {}
void BK4819_PlayCTCSSTail(void) {}
uint16_t BK4819_GetRSSI(void) { return 0; }
int8_t BK4819_GetRxGain_dB(void) { return 0; }
int16_t BK4819_GetRSSI_dBm(void) { return -120; }
uint8_t BK4819_GetGlitchIndicator(void) { return 0; }
uint8_t BK4819_GetExNoiceIndicator(void) { return 0; }
uint16_t BK4819_GetVoiceAmplitudeOut(void) { return 0; }
uint8_t BK4819_GetAfTxRx(void) { return 0; }
bool BK4819_GetFrequencyScanResult(uint32_t *f) { (void)f; return false; }
BK4819_CssScanResult_t BK4819_GetCxCSSScanResult(uint32_t *c, uint16_t *d) { (void)c;(void)d; return BK4819_CSS_RESULT_NOT_FOUND; }
void BK4819_DisableFrequencyScan(void) {}
void BK4819_EnableFrequencyScan(void) {}
void BK4819_SetScanFrequency(uint32_t f) { (void)f; }
void BK4819_Disable(void) {}
void BK4819_StopScan(void) {}
uint8_t BK4819_GetDTMF_5TONE_Code(void) { return 0; }
uint8_t BK4819_GetCDCSSCodeType(void) { return 0; }
uint8_t BK4819_GetCTCShift(void) { return 0; }
uint8_t BK4819_GetCTCType(void) { return 0; }
void BK4819_SendFSKData(uint16_t *p) { (void)p; }
void BK4819_PrepareFSKReceive(void) {}
void BK4819_PlayRoger(void) {}
void BK4819_PlayMDC1200(const uint8_t *d,const unsigned int s,const bool l) { (void)d;(void)s;(void)l; }
void BK4819_EnableMDC1200Rx(void) {}
void BK4819_DisableMDC1200Rx(void) {}
bool BK4819_ReadMDC1200RxBuffer(uint8_t *d, unsigned int *s) { (void)d;(void)s; return false; }
void BK4819_Enable_AfDac_DiscMode_TxDsp(void) {}
void BK4819_GetVoxAmp(uint16_t *r) { *r = 0; }
void BK4819_SetScrambleFrequencyControlWord(uint32_t f) { (void)f; }
void BK4819_PlayDTMFEx(bool b, char c) { (void)b;(void)c; }
#ifdef ENABLE_AIRCOPY
void BK4819_SetupAircopy(void) {}
#endif
#ifdef ENABLE_BYP_RAW_DEMODULATORS
void BK4819_EnterBypass(void) {}
void BK4819_EnterRaw(void) {}
#endif
