#include "board.h"
#include "driver/st7565.h"
#include "driver/py25q16.h"
#include "helper/battery.h"
#include <stdint.h>

void BOARD_FLASH_Init(void) {}
void BOARD_GPIO_Init(void) {}
void BOARD_ADC_Init(void) {}
void BOARD_ADC_GetBatteryInfo(uint16_t *v, uint16_t *c) { *v = 4000; *c = 100; }
void BOARD_Init(void) { ST7565_Init(); PY25Q16_Init(); }
