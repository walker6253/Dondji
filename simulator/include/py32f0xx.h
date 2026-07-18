#ifndef SIM_PY32F0XX_H
#define SIM_PY32F0XX_H
#include <stdint.h>
#include <stdbool.h>
typedef uint32_t uint32_t_alias;
#define __IO volatile
#define __I  volatile const
#define __O  volatile
#define IOPORT_BASE 0
typedef struct { uint32_t DATA; } GPIO_TypeDef;
#define GPIOA ((void*)0)
#define GPIOB ((void*)1)
#define GPIOC ((void*)2)
#define GPIOF ((void*)3)
#define SysTick_IRQn 15
static inline void NVIC_EnableIRQ(int irq) { (void)irq; }
static inline void NVIC_DisableIRQ(int irq) { (void)irq; }
extern int Key;
#endif
