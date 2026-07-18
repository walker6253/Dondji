#include "driver/system.h"
#include <unistd.h>
void SYSTEM_DelayMs(uint32_t Delay) { usleep(Delay * 1000); }
void SYSTEM_ConfigureClocks(void) {}
