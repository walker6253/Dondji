#include "driver/uart.h"
uint8_t UART_DMA_Buffer[256];
void UART_Init(void) {}
void UART_Send(const void *pBuffer, uint32_t Size) { (void)pBuffer;(void)Size; }
void UART_LogSend(const void *pBuffer, uint32_t Size) { (void)pBuffer;(void)Size; }
#ifdef ENABLE_FEAT_F4HWN_SCREENSHOT
bool UART_IsCableConnected(void) { return false; }
#endif
