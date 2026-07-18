#ifndef SIM_LL_SPI_H
#define SIM_LL_SPI_H
#include <stdint.h>
#define LL_SPI_FULL_DUPLEX 0
#define LL_SPI_MODE_MASTER 1
#define LL_SPI_DATAWIDTH_8BIT 0
#define LL_SPI_POLARITY_HIGH 1
#define LL_SPI_PHASE_2EDGE 1
#define LL_SPI_NSS_SOFT 0
#define LL_SPI_MSB_FIRST 0
#define LL_SPI_CRCCALCULATION_DISABLE 0
#define LL_SPI_BAUDRATEPRESCALER_DIV64 5
#define LL_SPI_DIRECTION_1LINE 0
typedef struct { uint32_t TransferDirection; uint32_t Mode; uint32_t DataWidth; uint32_t ClockPolarity; uint32_t ClockPhase; uint32_t NSS; uint32_t BitOrder; uint32_t CRCCalculation; uint32_t BaudRate; } LL_SPI_InitTypeDef;
typedef uint32_t SPI_TypeDef;
#define SPI1 0
static inline void LL_SPI_StructInit(LL_SPI_InitTypeDef *s) { (void)s; }
static inline void LL_SPI_Init(SPI_TypeDef s, LL_SPI_InitTypeDef *i) { (void)s;(void)i; }
static inline void LL_SPI_Enable(SPI_TypeDef s) { (void)s; }
static inline bool LL_SPI_IsActiveFlag_TXE(SPI_TypeDef s) { (void)s; return true; }
static inline void LL_SPI_TransmitData8(SPI_TypeDef s, uint8_t d) { (void)s;(void)d; }
static inline bool LL_SPI_IsActiveFlag_RXNE(SPI_TypeDef s) { (void)s; return true; }
static inline uint8_t LL_SPI_ReceiveData8(SPI_TypeDef s) { (void)s; return 0; }
#endif
