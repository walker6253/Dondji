#include "audio.h"
void AUDIO_PlayBeep(BEEP_Type_t Beep) { (void)Beep; }
void AUDIO_PlaySingleVoice(bool bFlag) { (void)bFlag; }
void AUDIO_SetVoiceID(uint8_t Index, VOICE_ID_t VoiceID) { (void)Index;(void)VoiceID; }
uint8_t AUDIO_SetDigitVoice(uint8_t Index, uint16_t Value) { (void)Index;(void)Value; return 0; }
void AUDIO_PlayQueuedVoice(void) {}
