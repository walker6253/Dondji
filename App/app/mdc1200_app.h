/*
 * Dondji Firmware — MDC1200 RX application layer
 *
 * [软接收 2026-10-08] RX 为软件 FFSK 解调(PA4 采 FM 鉴频音频, ADC@14.4kHz
 * + 软件解调前端), 不占用 BK4819 FSK modem — 无线中断钩子(AppEnableRx/
 * AppOnRadioInterrupt)已删除。采样启停由静噪门控自动管理(AppTick10ms)。
 */
#ifndef APP_MDC1200_APP_H
#define APP_MDC1200_APP_H

#include <stdbool.h>
#include <stdint.h>

#include "app/mdc_addrbook.h"

extern uint16_t gMdcId_RX;
extern uint8_t  gMdcId_RX_timeout;
extern char     gMdcCallsign[MDC_ADDRBOOK_NAME_LEN + 1];

bool MDC1200_AppRxEnabled(void);
void MDC1200_AppDisableRx(void) __attribute__((noinline));  /* 入口停采样; noinline: 防LTO多调用点内联展开 */
void MDC1200_AppTick10ms(void);      /* 静噪门控采样启停+泄流+解码 */
void MDC1200_AppTick500ms(void);     /* 弹窗超时+自收抑制超时 */
void MDC1200_AppNoteOwnTx(void);     /* MDC TX尾部: 设自收抑制窗 */
bool MDC1200_AppRxSampling(void);    /* 电池采样门控用 */

#endif
