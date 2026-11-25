#pragma once
/*
 * Board-specific constants (esp32-boards). The hardware type is selected by a
 * -DBOARD_xxx define and the physical module by -DBOARD_MODULE_ID=nn, both set in
 * boards.ini. Pure preprocessor macros: usable from ESP-IDF and Arduino, C and C++.
 *
 * Every board block defines:
 *   BOARD_NAME            human-readable description
 *   BOARD_FLASH_MB        flash size in MB
 *   BOARD_PSRAM_MB        PSRAM size in MB (0 = none)
 *   BOARD_HAS_NATIVE_USB  1 = chip has USB-OTG / USB-Serial-JTAG (ESP32-S3)
 *   BOARD_PIN_LED         user LED GPIO, -1 = none
 *   BOARD_LED_IS_RGB      1 = WS2812 RGB LED, 0 = plain LED
 *   BOARD_PIN_BUTTON      user/BOOT button GPIO (active low), -1 = none
 * plus optional feature flags (BOARD_HAS_MODEM, BOARD_HAS_SD_CARD, BOARD_HAS_CAMERA_PORT)
 * and their pins. Apps state their requirements with #if checks, e.g.
 *   #if BOARD_PSRAM_MB < 2
 *   #error "This app needs at least 2 MB PSRAM"
 *   #endif
 */

#ifndef BOARD_MODULE_ID
    #define BOARD_MODULE_ID     0
#endif

/* 1 = external antenna via U.FL connector (e.g. WROOM-1U), 0 = on-board PCB antenna */
#ifndef BOARD_ANTENNA_EXTERNAL
    #define BOARD_ANTENNA_EXTERNAL  0
#endif

/* ---------------------------------------------------------------------------- */
#if defined(BOARD_DOIT_ESP32_DEVKIT_V1)
    #define BOARD_NAME          "DOIT ESP32 DEVKIT V1 (WROOM-32), on-board antenna"
    #define BOARD_PIN_LED       2       // blue on-board LED
    #define BOARD_LED_IS_RGB    0
    #define BOARD_PIN_BUTTON    0       // BOOT button (active low)
    #define BOARD_FLASH_MB      4
    #define BOARD_PSRAM_MB      0
    #define BOARD_HAS_NATIVE_USB 0

/* ---------------------------------------------------------------------------- */
#elif defined(BOARD_ESP32_DEVKIT_V1_38PIN)
    #define BOARD_NAME          "ESP32 DEVKIT V1 38-pin (WROOM-32U), external antenna"
    // GPIO6-11 are wired to the SPI flash (exposed on the 38-pin header) - don't use them.
    #define BOARD_PIN_LED       (-1)    // no user LED - only the red power LED (verified)
    #define BOARD_LED_IS_RGB    0
    #define BOARD_PIN_BUTTON    0       // BOOT button (active low)
    #define BOARD_FLASH_MB      4
    #define BOARD_PSRAM_MB      0
    #define BOARD_HAS_NATIVE_USB 0

/* ---------------------------------------------------------------------------- */
#elif defined(BOARD_S3_DEVKITC1_N16R8)
    // 44-pin ESP32-S3-DevKitC-1 clones (CH343 USB-UART), WROOM-1 / WROOM-1U module
    #if BOARD_ANTENNA_EXTERNAL
        #define BOARD_NAME      "ESP32-S3-DevKitC-1U N16R8 44-pin clone, external antenna"
    #else
        #define BOARD_NAME      "ESP32-S3-DevKitC-1 N16R8 44-pin clone, on-board antenna"
    #endif
    #define BOARD_PIN_LED       48      // WS2812 RGB LED (verified on Modules 14-18)
    #define BOARD_LED_IS_RGB    1
    #define BOARD_PIN_BUTTON    0       // BOOT button (active low)
    #define BOARD_FLASH_MB      16
    #define BOARD_PSRAM_MB      8
    #define BOARD_HAS_NATIVE_USB 1

/* ---------------------------------------------------------------------------- */
#elif defined(BOARD_S3_DEVKITC1_N32R16V)
    // Genuine Espressif 44-pin board, ESP32-S3-WROOM-2 module, on-board antenna
    #define BOARD_NAME          "Espressif ESP32-S3-DevKitC-1 N32R16V 44-pin, on-board antenna"
    // Octal-flash/PSRAM module: GPIO35-37 are used by the flash/PSRAM bus.
    #define BOARD_PIN_LED       38      // WS2812 RGB LED (verified on Module 19; DevKitC-1 v1.0 used GPIO48)
    #define BOARD_LED_IS_RGB    1
    #define BOARD_PIN_BUTTON    0       // BOOT button (active low)
    #define BOARD_FLASH_MB      32
    #define BOARD_PSRAM_MB      16
    #define BOARD_HAS_NATIVE_USB 1

/* ---------------------------------------------------------------------------- */
#elif defined(BOARD_LILYGO_T_SIM7080G_S3)
    // "Standard" variant WITHOUT the AXP2101 PMU. Pins from LILYGO's
    // "T-SIM7080G-S3 Standard PINOUT" diagram. (LilyGO's GitHub examples are
    // for the PMU variant and use DIFFERENT modem pins - don't copy them.)
    #define BOARD_NAME          "LILYGO T-SIM7080G-S3 Standard (no PMU)"
    #define BOARD_PIN_LED       (-1)    // no user LED
    #define BOARD_LED_IS_RGB    0
    #define BOARD_PIN_BUTTON    0       // BOOT button (active low)
    #define BOARD_FLASH_MB      16
    #define BOARD_PSRAM_MB      8       // not yet verified with esptool
    #define BOARD_HAS_NATIVE_USB 1

    #define BOARD_HAS_MODEM     1
    #define BOARD_HAS_SD_CARD   1
    #define BOARD_HAS_CAMERA_PORT 1

    // SIM7080G modem (UART pins from the ESP32-S3's point of view)
    #define BOARD_MODEM_PWR_PIN 46      // PWRKEY (GPIO46 is a strapping pin)
    #define BOARD_MODEM_DTR_PIN 7
    #define BOARD_MODEM_RI_PIN  6       // RING
    #define BOARD_MODEM_RX_PIN  4       // ESP RX  <- modem TXD
    #define BOARD_MODEM_TX_PIN  5       // ESP TX  -> modem RXD

    // I2C on the 2x STEMMA QT / Qwiic connectors
    #define BOARD_I2C_SDA       3
    #define BOARD_I2C_SCL       2

    // Analog inputs (voltage dividers)
    #define BOARD_BAT_ADC_PIN   8       // 18650 battery voltage
    #define BOARD_SOLAR_ADC_PIN 18      // solar input (5-6 V) voltage

    // TF (microSD) card, SPI mode
    #define BOARD_SD_SCK        12
    #define BOARD_SD_MISO       13
    #define BOARD_SD_MOSI       11
    #define BOARD_SD_CS         10

    // Optional camera (DVP). NOTE: Y2/Y6/PCLK use GPIO35/36/37, which octal
    // PSRAM also needs - the camera and octal PSRAM cannot be used together.
    #define BOARD_CAM_PWDN      (-1)
    #define BOARD_CAM_RESET     (-1)    // NC
    #define BOARD_CAM_XCLK      21
    #define BOARD_CAM_SIOD      1       // SCCB SDA
    #define BOARD_CAM_SIOC      41      // SCCB SCL
    #define BOARD_CAM_VSYNC     9
    #define BOARD_CAM_HREF      14
    #define BOARD_CAM_PCLK      37
    #define BOARD_CAM_Y9        40
    #define BOARD_CAM_Y8        39
    #define BOARD_CAM_Y7        38
    #define BOARD_CAM_Y6        36
    #define BOARD_CAM_Y5        16
    #define BOARD_CAM_Y4        47
    #define BOARD_CAM_Y3        15
    #define BOARD_CAM_Y2        35

/* ---------------------------------------------------------------------------- */
#else
    #error "No board selected: add -DBOARD_xxx to build_flags in platformio.ini"
#endif

/* Optional features default to "not present" */
#ifndef BOARD_HAS_MODEM
    #define BOARD_HAS_MODEM         0
#endif
#ifndef BOARD_HAS_SD_CARD
    #define BOARD_HAS_SD_CARD       0
#endif
#ifndef BOARD_HAS_CAMERA_PORT
    #define BOARD_HAS_CAMERA_PORT   0
#endif
