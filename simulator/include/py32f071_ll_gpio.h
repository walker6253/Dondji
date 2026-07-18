#ifndef SIM_LL_GPIO_H
#define SIM_LL_GPIO_H
#include <stdint.h>
#include <stdbool.h>
#include "py32f0xx.h"
#define LL_GPIO_PIN_0   0x0001
#define LL_GPIO_PIN_1   0x0002
#define LL_GPIO_PIN_2   0x0004
#define LL_GPIO_PIN_3   0x0008
#define LL_GPIO_PIN_4   0x0010
#define LL_GPIO_PIN_5   0x0020
#define LL_GPIO_PIN_6   0x0040
#define LL_GPIO_PIN_7   0x0080
#define LL_GPIO_PIN_8   0x0100
#define LL_GPIO_PIN_9   0x0200
#define LL_GPIO_PIN_10  0x0400
#define LL_GPIO_PIN_11  0x0800
#define LL_GPIO_PIN_12  0x1000
#define LL_GPIO_PIN_13  0x2000
#define LL_GPIO_PIN_14  0x4000
#define LL_GPIO_PIN_15  0x8000
#define LL_GPIO_MODE_INPUT        0
#define LL_GPIO_MODE_OUTPUT       1
#define LL_GPIO_MODE_ALTERNATE    2
#define LL_GPIO_MODE_ANALOG       3
#define LL_GPIO_OUTPUT_PUSHPULL   0
#define LL_GPIO_OUTPUT_OPENDRAIN  1
#define LL_GPIO_PULL_NO   0
#define LL_GPIO_PULL_UP   1
#define LL_GPIO_PULL_DOWN 2
#define LL_GPIO_SPEED_FREQ_LOW        0
#define LL_GPIO_SPEED_FREQ_MEDIUM     1
#define LL_GPIO_SPEED_FREQ_HIGH       2
#define LL_GPIO_SPEED_FREQ_VERY_HIGH  3
#define LL_GPIO_AF0_SPI1   0
typedef struct { uint32_t Pin; uint32_t Mode; uint32_t Pull; uint32_t Speed; uint32_t OutputType; uint32_t Alternate; } LL_GPIO_InitTypeDef;
static inline void LL_GPIO_StructInit(LL_GPIO_InitTypeDef *s) { s->Pin=0;s->Mode=0;s->Pull=0;s->Speed=0;s->OutputType=0;s->Alternate=0; }
static inline void LL_GPIO_Init(GPIO_TypeDef *p, LL_GPIO_InitTypeDef *s) { (void)p; (void)s; }
static inline void LL_GPIO_SetOutputPin(GPIO_TypeDef *p, uint32_t m) { (void)p; (void)m; }
static inline void LL_GPIO_ResetOutputPin(GPIO_TypeDef *p, uint32_t m) { (void)p; (void)m; }
static inline void LL_GPIO_TogglePin(GPIO_TypeDef *p, uint32_t m) { (void)p; (void)m; }
static inline bool LL_GPIO_IsInputPinSet(GPIO_TypeDef *p, uint32_t m) { (void)p; (void)m; return false; }
#endif
