# esp32-boards

Shared board definitions for PlatformIO ESP32 / ESP32-S3 projects. This repo holds the hardware types, the inventory of physical boards ("modules"), and the pins and capabilities of each board, in one place. Projects include it as a git submodule, so one application can be built for any board in the inventory by picking a PlatformIO environment.

It works with both frameworks of the `espressif32` platform:
- **ESP-IDF**
- **Arduino**

## Contents

| Path | Purpose |
|---|---|
| `boards.ini` | Hardware types (`[hw_*]`) and physical modules (`[env:mNN_*]`) |
| `include/board.h` | Board name, pins and capabilities (`BOARD_FLASH_MB`, `BOARD_PSRAM_MB`, `BOARD_HAS_*`) as preprocessor macros |
| `include/board_led.h`, `src/board_led.c` | On-board LED driver (plain LED, WS2812 RGB, or none) |
| `sdkconfig/` | ESP-IDF defaults per hardware type |
| `examples/arduino-blink/` | Minimal Arduino project using this layer |
| `templates/CLAUDE.md.template` | Starting point for each project's `CLAUDE.md` (Claude Code context) |

## Modules

| Environment | Board | Flash / PSRAM | User LED |
|---|---|---|---|
| `m12_doit_esp32`, `m13_doit_esp32` | ESP32 DEVKIT V1 38-pin clone (WROOM-32U, external antenna); 13 is a deployed production unit | 4 MB / – | none |
| `m14_esp32s3` … `m16_esp32s3` | ESP32-S3-DevKitC-1 N16R8 44-pin clone (on-board antenna) | 16 MB quad / 8 MB | WS2812, GPIO48 |
| `m17_esp32s3`, `m18_esp32s3` | ESP32-S3-DevKitC-1U N16R8 44-pin clone (external antenna) | 16 MB quad / 8 MB | WS2812, GPIO48 |
| `m19_esp32s3` | Espressif ESP32-S3-DevKitC-1 N32R16V (genuine) | 32 MB octal / 16 MB | WS2812, GPIO38 |
| `m20_lilygo_sim7080g` | LILYGO T-SIM7080G-S3 Standard (no PMU), SIM7080G modem | 16 MB quad / 8 MB | none |

## Using it in a project

```sh
git submodule add https://github.com/TomHildebrand/esp32-boards.git boards
```

Then make these changes to `platformio.ini`:

```ini
[platformio]
extra_configs = boards/boards.ini
default_envs = m15_esp32s3
build_dir = C:/pio-build/<project-name>    ; optional: keep builds out of synced folders

[boards]
dir = boards

[env]
framework = arduino          ; or espidf
build_flags =                ; app-wide flags, added for every module
lib_deps =                   ; app libraries
```

**ESP-IDF** projects also need a `sdkconfig.defaults` in the project root for app settings. It may be empty, and it is applied after the board defaults.

The board sources (`boards/src/`) are compiled automatically for both frameworks. In the code, `#include "board.h"` and use its macros instead of hard-coded pin numbers.

After cloning a project, run `git submodule update --init`. To pick up board changes later, run `git submodule update --remote boards` and commit the result.

## Porting an existing Arduino project

1. Add the submodule and replace `platformio.ini` as shown above. Keep the project's `lib_deps`, and move its own `build_flags` into `[env]`.
2. Sort the hard-coded pin numbers into two groups:
   - **Pins that belong to the board** (LED, BOOT button, built-in modem, SD slot) → use the `board.h` macros.
   - **Pins for parts you wired up yourself** (sensors, displays, relays) → put them in a project header such as `app_pins.h`. Use one block per hardware type where they differ:

     ```c
     #if defined(BOARD_S3_DEVKITC1_N16R8) || defined(BOARD_S3_DEVKITC1_N32R16V)
         #define PIN_DHT22   4
     #elif defined(BOARD_ESP32_DEVKIT_V1_38PIN)
         #define PIN_DHT22   15
     #else
         #error "Wiring for this board is not defined"
     #endif
     ```
3. State what the app needs, so an unsuitable board fails at compile time and not at runtime:

   ```c
   #if BOARD_PSRAM_MB < 2
   #error "This app needs PSRAM"
   #endif
   ```
4. Build for each module you intend to use (`pio run -e mNN_...`).
5. Add a `CLAUDE.md` to the project root, made from [templates/CLAUDE.md.template](templates/CLAUDE.md.template). Fill in:
   - production vs. test modules
   - which files hold secrets
   - the supported hardware
   - the commands

   Claude Code reads this file at the start of every session in that folder. Without it, a session knows nothing about the board layer or the safety rules. This applies to new projects too.

## Adding boards

- **New physical board of an existing type:** add an `[env:mNN_*]` section to `boards.ini`.
- **New hardware type:** add all of the following:
  - a `[hw_*]` section
  - a `sdkconfig/sdkconfig.defaults.<variant>` file
  - an `#elif` block in `include/board.h`
- Check a board's real flash and PSRAM with `esptool.py --port COMx flash_id` first. Board markings are not always accurate.
