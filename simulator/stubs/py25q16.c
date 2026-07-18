#include "driver/py25q16.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FLASH_SIZE (2 * 1024 * 1024)
const char *sim_flash_file = "sim_flash.bin";
static uint8_t *flash_data = NULL;
static bool dirty = false;

static void flush(void) {
    if (dirty && flash_data) {
        FILE *f = fopen(sim_flash_file, "wb");
        if (f) { fwrite(flash_data, 1, FLASH_SIZE, f); fclose(f); }
        dirty = false;
    }
}

void PY25Q16_Init(void) {
    flash_data = calloc(1, FLASH_SIZE);
    if (!flash_data) return;
    memset(flash_data, 0xFF, FLASH_SIZE);
    FILE *f = fopen(sim_flash_file, "rb");
    if (f) { fread(flash_data, 1, FLASH_SIZE, f); fclose(f); }
}

void PY25Q16_ReadBuffer(uint32_t addr, void *buf, uint32_t sz) {
    if (flash_data && addr + sz <= FLASH_SIZE) memcpy(buf, flash_data + addr, sz);
}

void PY25Q16_WriteBuffer(uint32_t addr, const void *buf, uint32_t sz) {
    if (flash_data && addr + sz <= FLASH_SIZE) {
        memcpy(flash_data + addr, buf, sz);
        dirty = true; flush();
    }
}

void PY25Q16_SectorErase(uint32_t addr) {
    if (flash_data && addr + 4096 <= FLASH_SIZE) {
        memset(flash_data + addr, 0xFF, 4096);
        dirty = true; flush();
    }
}
