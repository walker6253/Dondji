#ifndef SIM_LL_BUS_H
#define SIM_LL_BUS_H
#include <stdint.h>
#define LL_APB1_GRP2_PERIPH_SPI1  1
#define LL_IOP_GRP1_PERIPH_GPIOA  1
#define LL_IOP_GRP1_PERIPH_GPIOB  1
#define LL_IOP_GRP1_PERIPH_GPIOC  1
#define LL_IOP_GRP1_PERIPH_GPIOF  1
static inline void LL_APB1_GRP2_EnableClock(uint32_t x) { (void)x; }
static inline void LL_IOP_GRP1_EnableClock(uint32_t x) { (void)x; }
#endif
