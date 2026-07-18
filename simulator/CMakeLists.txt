cmake_minimum_required(VERSION 3.16)
project(dondji_sim C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

find_package(SDL2 REQUIRED)

set(APP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../App)

# Real firmware source files
set(REAL_SOURCES
    ${APP_DIR}/misc.c
    ${APP_DIR}/settings.c
    ${APP_DIR}/frequencies.c
    ${APP_DIR}/functions.c
    ${APP_DIR}/font.c
    ${APP_DIR}/bitmaps.c
    ${APP_DIR}/radio.c
    ${APP_DIR}/dcs.c
    ${APP_DIR}/scheduler.c
    ${APP_DIR}/helper/battery.c
    ${APP_DIR}/helper/boot.c
    ${APP_DIR}/app/app.c
    ${APP_DIR}/app/menu.c
    ${APP_DIR}/app/main.c
    ${APP_DIR}/app/scanner.c
    ${APP_DIR}/app/generic.c
    ${APP_DIR}/app/common.c
    ${APP_DIR}/app/chFrScanner.c
    ${APP_DIR}/app/dtmf.c
    ${APP_DIR}/app/mdc1200.c
    ${APP_DIR}/app/action.c
    ${APP_DIR}/ui/battery.c
    ${APP_DIR}/ui/helper.c
    ${APP_DIR}/ui/inputbox.c
    ${APP_DIR}/ui/main.c
    ${APP_DIR}/ui/menu.c
    ${APP_DIR}/ui/scanner.c
    ${APP_DIR}/ui/status.c
    ${APP_DIR}/ui/ui.c
    ${APP_DIR}/ui/welcome.c
    ${APP_DIR}/ui/menu_lang.c
    ${APP_DIR}/ui/menu_sub_values_cn.c
    ${APP_DIR}/ui/dualvfo_u8g2_freq.c
    ${APP_DIR}/external/printf/printf.c
    ${APP_DIR}/version.c
)

set(U8G2_DIR ${APP_DIR}/../external/U8G2/csrc)
set(U8G2_SOURCES
    ${U8G2_DIR}/u8g2_bitmap.c
    ${U8G2_DIR}/u8g2_box.c
    ${U8G2_DIR}/u8g2_buffer.c
    ${U8G2_DIR}/u8g2_d_memory_minimal.c
    ${U8G2_DIR}/u8g2_d_setup_minimal.c
    ${U8G2_DIR}/u8g2_font.c
    ${U8G2_DIR}/u8g2_hvline.c
    ${U8G2_DIR}/u8g2_ll_hvline.c
    ${U8G2_DIR}/u8g2_setup.c
    ${U8G2_DIR}/u8x8_8x8.c
    ${U8G2_DIR}/u8x8_byte.c
    ${U8G2_DIR}/u8x8_cad.c
    ${U8G2_DIR}/u8x8_display.c
    ${U8G2_DIR}/u8x8_gpio.c
    ${U8G2_DIR}/u8x8_setup.c
    ${U8G2_DIR}/u8x8_string.c
    ${U8G2_DIR}/u8x8_d_st7565.c
)

set(STUB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/stubs)
set(STUB_SOURCES
    ${STUB_DIR}/gpio.c
    ${STUB_DIR}/system.c
    ${STUB_DIR}/systick.c
    ${STUB_DIR}/backlight.c
    ${STUB_DIR}/bk4819.c
    ${STUB_DIR}/bk1080.c
    ${STUB_DIR}/py25q16.c
    ${STUB_DIR}/flash.c
    ${STUB_DIR}/uart.c
    ${STUB_DIR}/vcp.c
    ${STUB_DIR}/voice.c
    ${STUB_DIR}/i2c.c
    ${STUB_DIR}/crc.c
    ${STUB_DIR}/eeprom_compat.c
    ${STUB_DIR}/keyboard.c
    ${STUB_DIR}/st7565.c
    ${STUB_DIR}/board_stub.c
    ${STUB_DIR}/am_fix.c
    ${STUB_DIR}/audio_stub.c
    ${STUB_DIR}/screenshot_stub.c
    ${STUB_DIR}/boot_logo_stub.c
    ${STUB_DIR}/app_fm.c
    ${STUB_DIR}/app_spectrum.c
    ${STUB_DIR}/app_aircopy.c
    ${STUB_DIR}/app_rega.c
    ${STUB_DIR}/app_flashlight.c
    ${STUB_DIR}/app_mokuyu.c
    ${STUB_DIR}/app_toolbox_menu.c
    ${STUB_DIR}/app_cw.c
    ${STUB_DIR}/ui_fmradio.c
    ${STUB_DIR}/ui_aircopy.c
    ${STUB_DIR}/ui_lock.c
    ${STUB_DIR}/missing_symbols.c
)

add_executable(dondji_sim
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${REAL_SOURCES}
    ${U8G2_SOURCES}
    ${STUB_SOURCES}
)

# IMPORTANT: App/ FIRST so real headers win over stubs
target_include_directories(dondji_sim PRIVATE
    ${APP_DIR}
    ${U8G2_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(dondji_sim PRIVATE
    PRINTF_INCLUDE_CONFIG_H
    ENABLE_FEAT_F4HWN
    ENABLE_CHINESE
    ENABLE_UART
    ENABLE_FEAT_F4HWN_SCREENSHOT
    ENABLE_FEAT_F4HWN_GAME
    ENABLE_SPECTRUM
    ENABLE_FMRADIO
    ENABLE_FEAT_F4HWN_RX_TX_TIMER
    ENABLE_FEAT_F4HWN_CHARGING_C
    ENABLE_FEAT_F4HWN_SLEEP
    ENABLE_FEAT_F4HWN_RESUME_STATE
    ENABLE_FEAT_F4HWN_NARROWER
    ENABLE_FEAT_F4HWN_INV
    ENABLE_FEAT_F4HWN_CTR
    ENABLE_FEAT_F4HWN_RESCUE_OPS
    ENABLE_FEAT_F4HWN_VOL
    ENABLE_FEAT_F4HWN_AUDIO
    ENABLE_FEAT_F4HWN_RESET_VFO
    ENABLE_FEAT_F4HWN_PMR
    ENABLE_FEAT_F4HWN_CA
    ENABLE_FEAT_F4HWN_DEBUG
    ENABLE_FEAT_F4HWN_MEM
    ENABLE_SCAN_RANGES
    ENABLE_BIG_FREQ
    ENABLE_SMALL_BOLD
    ENABLE_RSSI_BAR
    ENABLE_AUDIO_BAR
    ENABLE_FEAT_F4HWN_AUDIO_SCOPE
    ENABLE_VOX
    ENABLE_ALARM
    ENABLE_TX1750
    ENABLE_PWRON_PASSWORD
    ENABLE_DTMF_CALLING
    ENABLE_FLASHLIGHT
    ENABLE_NOAA
    ENABLE_VOICE
    ENABLE_KEEP_MEM_NAME
    ENABLE_WIDE_RX
    ENABLE_TX_WHEN_AM
    ENABLE_F_CAL_MENU
    ENABLE_CTCSS_TAIL_PHASE_SHIFT
    ENABLE_BOOT_BEEPS
    ENABLE_SHOW_CHARGE_LEVEL
    ENABLE_REVERSE_BAT_SYMBOL
    ENABLE_SQUELCH_MORE_SENSITIVE
    ENABLE_FASTER_CHANNEL_SCAN
    ENABLE_COPY_CHAN_TO_VFO
    ENABLE_REDUCE_LOW_MID_TX_POWER
    ENABLE_BYP_RAW_DEMODULATORS
    ENABLE_BLMIN_TMP_OFF
    ENABLE_EXTRA_UART_CMD
    ENABLE_REGA
    ENABLE_AIRCOPY
    ENABLE_NO_CODE_SCAN_TIMEOUT
    ENABLE_CUSTOM_MENU_LAYOUT
    ENABLE_FEAT_F4HWN_SPECTRUM
    U8G2_NO_BALANCED_STR_WIDTH_CALCULATION
    U8G2_WITHOUT_UNICODE
    U8G2_WITHOUT_FONT_ROTATION
    U8G2_WITHOUT_CLIP_WINDOW_SUPPORT
    U8G2_WITHOUT_INTERSECTION
    U8G2_WITHOUT_HVLINE_SPEED_OPTIMIZATION
    U8X8_WITHOUT_SET_CONTRAST
    AUTHOR_STRING_1="EGZUMER"
    AUTHOR_STRING_2="F4HWN"
    VERSION_STRING_1="v0.22"
    VERSION_STRING_2="v4.2"
    EDITION_STRING="Custom"
    AUTHOR_STRING="EGZUMER+F4HWN"
    VERSION_STRING="v4.2"
    BUILD_COMMIT="simulator"
    SQL_TONE=550
    ALERT_TOT=10
    MENU_SCR=999
)

# Strip ARM attributes for native build

target_link_libraries(dondji_sim PRIVATE SDL2::SDL2)

if(APPLE)
    target_link_libraries(dondji_sim PRIVATE "-framework Foundation")
endif()

# Pre-include stdbool.h for headers that forget it
target_compile_options(dondji_sim PRIVATE -include stdbool.h
    -Wno-unused-parameter
    -Wno-unused-function
    -Wno-unused-variable
    -Wno-sign-compare
    -Wno-pointer-sign
    -Wno-int-conversion
    -Wno-missing-braces
    -Wno-incompatible-pointer-types-discards-qualifiers
    -Wno-implicit-function-declaration
)
