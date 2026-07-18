#include "driver/st7565.h"
#include <string.h>

uint8_t gStatusLine[LCD_WIDTH];
uint8_t gFrameBuffer[FRAME_LINES][LCD_WIDTH];

// Simulator render callback
typedef void (*sim_render_cb_t)(const uint8_t status[128], const uint8_t frame[8][128]);
sim_render_cb_t sim_render_callback = NULL;

void ST7565_DrawLine(const unsigned int Column, const unsigned int Line,
                     const uint8_t *pBitmap, const unsigned int Size)
{ (void)Column;(void)Line;(void)pBitmap;(void)Size; }

void ST7565_BlitFullScreen(void)
{
    if (sim_render_callback)
        sim_render_callback(gStatusLine, gFrameBuffer);
}

void ST7565_BlitFullScreenDualVfoTightTop(void) { ST7565_BlitFullScreen(); }
void ST7565_BlitLine(unsigned line) { (void)line; }
void ST7565_BlitStatusLine(void) {}
void ST7565_FillScreen(uint8_t Value)
{
    memset(gStatusLine, Value, LCD_WIDTH);
    for (int i = 0; i < FRAME_LINES; i++)
        memset(gFrameBuffer[i], Value, LCD_WIDTH);
}
void ST7565_Init(void) { ST7565_FillScreen(0x00); }
void ST7565_ShutDown(void) {}
void ST7565_FixInterfGlitch(void) {}
void ST7565_HardwareReset(void) {}
void ST7565_SelectColumnAndLine(uint8_t c, uint8_t l) { (void)c;(void)l; }
void ST7565_WriteByte(uint8_t v) { (void)v; }
void ST7565_ContrastAndInv(void) {}
void ST7565_Gauge(uint8_t l, uint8_t min, uint8_t max, uint8_t val) { (void)l;(void)min;(void)max;(void)val; }
int16_t map(int16_t x, int16_t a, int16_t b, int16_t c, int16_t d)
{
    if (x <= a) return c; if (x >= b) return d;
    return c + (int32_t)(x - a) * (d - c) / (b - a);
}
