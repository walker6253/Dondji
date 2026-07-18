#include <stdint.h>
#include "driver/flash.h"
void FLASH_Init(FLASH_READ_MODE ReadMode) { (void)ReadMode; }
void FLASH_ConfigureTrimValues(void) {}
uint32_t FLASH_ReadNvrWord(uint32_t Address) { (void)Address; return 0xFFFFFFFF; }
