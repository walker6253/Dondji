#ifndef SIM_LL_CORTEX_H
#define SIM_LL_CORTEX_H
#define SysTick_IRQn 15
static inline void NVIC_EnableIRQ(int irq) { (void)irq; }
static inline void NVIC_DisableIRQ(int irq) { (void)irq; }
#endif
