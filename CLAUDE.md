# esp32-boards

Shared board layer for PlatformIO ESP32 / ESP32-S3 projects: hardware types, the inventory of physical
boards ("modules"), per-board pins and constants. Each project includes this repo as a **git submodule
at `boards/`**. Works with both frameworks of the `espressif32` platform:
- **ESP-IDF 6.0.1**
- **Arduino core 2.0.17**, which runs on ESP-IDF 4.4.

Host is Windows; `pio` is not on PATH.

**Change board data here, not inside a project.** Commit and push it in this repo, then run
`git submodule update --remote boards` in each project.

## Files

| Path | Purpose |
|---|---|
| `boards.ini` | `[boards_base]`, `[hw_*]` hardware types, `[env:mNN_*]` physical modules. Pulled in by a project via `extra_configs`. |
| `include/board.h` | `BOARD_xxx` → `BOARD_NAME`, pins, `BOARD_FLASH_MB` / `BOARD_PSRAM_MB` / `BOARD_HAS_*` capabilities. Pure macros (C/C++, both frameworks). `#error` if no board is selected. |
| `include/board_led.h`, `src/board_led.c` | On-board LED driver: ESP-IDF RMT or Arduino `neopixelWrite()`/`digitalWrite()`, chosen by `#ifdef ARDUINO`. |
| `sdkconfig/sdkconfig.defaults.<variant>` | ESP-IDF settings per hardware type (flash size/mode, PSRAM, USB console). Ignored by Arduino builds. |
| `examples/arduino-blink/` | Arduino LED test using this layer. Its `[boards] dir = ../..` points at the repo root. |

## How a project uses it

- The project's `platformio.ini` needs:
  - `extra_configs = boards/boards.ini`
  - `[boards] dir = boards`
  - an `[env]` with `framework` (`espidf` or `arduino`) and `build_flags` (may be empty). Module envs append `${env.build_flags}`.
- ESP-IDF projects also need their own `sdkconfig.defaults` in the project root (may be empty). It is listed **last** in `SDKCONFIG_DEFAULTS`, so app settings override board settings.
- Both frameworks compile `boards/src/` through `build_src_filter` (set in `[boards_base]`), with the project's build flags. **Don't** also add the board sources in an ESP-IDF project's `src/CMakeLists.txt`: they would be compiled twice and fail to link with "multiple definition".
- `boards.ini` defines **no** `[env]` or `[platformio]` section, so it never overrides the project's own.
- `build_flags` that one hardware type adds for one framework (`-DBOARD_HAS_PSRAM`, `-DARDUINO_USB_CDC_ON_BOOT=1`) are harmless under the other.
- `board_build.arduino.memory_type` only affects Arduino builds; `cmake_extra_args` only ESP-IDF builds.

## Modules

| Env | Module | Board | Memory (verified with esptool) | User LED | UART port (flash + console) | Native USB port (debug/JTAG) |
|---|---|---|---|---|---|---|
| `m12_doit_esp32` | 12 | ESP32 DEVKIT V1 **38-pin** clone, WROOM-**32U**, **external antenna** (U.FL) | ESP32-D0WD-V3, 4 MB flash, no PSRAM | **none** (verified; red power LED only) | COM4 (CP2102) | – |
| `m13_doit_esp32` | 13 | Same as Module 12. **Production unit** (Oliver Water Meter Controller), deployed at Oliver; updated over the air. **Uploads from PlatformIO are blocked** (custom `upload_command` that always fails) | same as 12 (not verified with esptool) | none | – (not on this PC) | – |
| `m14_esp32s3` | 14 | ESP32-S3-DevKitC-1 N16R8 **44-pin** clone, on-board antenna | 16 MB quad 3.3 V + 8 MB PSRAM | WS2812 GPIO48 (verified) | COM10 | COM5 |
| `m15_esp32s3` | 15 | ESP32-S3-DevKitC-1 N16R8 44-pin clone, on-board antenna | same | WS2812 GPIO48 (verified) | COM7 | COM6 |
| `m16_esp32s3` | 16 | ESP32-S3-DevKitC-1 N16R8 44-pin clone, on-board antenna | same | WS2812 GPIO48 (verified) | COM9 | COM8 |
| `m17_esp32s3` | 17 | ESP32-S3-DevKitC-1**U** N16R8 44-pin clone, **external antenna** (U.FL) | same | WS2812 GPIO48 (verified) | COM12 | COM17 |
| `m18_esp32s3` | 18 | ESP32-S3-DevKitC-1**U** N16R8 44-pin clone, **external antenna** (U.FL) | same | WS2812 GPIO48 (verified) | COM13 | COM18 |
| `m19_esp32s3` | 19 | **Genuine Espressif** ESP32-S3-DevKitC-1 N32R16V, 44-pin, on-board antenna | 32 MB octal flash + 16 MB octal PSRAM (1.8 V) | WS2812 GPIO38 (verified) | COM14 (CP210x) | COM16 |
| `m20_lilygo_sim7080g` | 20 | LILYGO T-SIM7080G-S3, variant **without PMU** (WROOM-1 N16R8, SIM7080G) | 16 MB quad + 8 MB PSRAM (not verified) | none | not set yet | – |

Modules 14–19 have two USB plugs. On 14–18 (clones) the right plug is the CH343 USB-UART, the left one native USB. On 19 (genuine Espressif) the left plug is the CP210x USB-UART, the right one native USB. Port numbers were verified on the development PC on 2026-11-25. `upload_port`/`monitor_port` use the UART port; the native USB port is recorded as `custom_usb_port` (information only).

How the hardware types relate:
- `[hw_s3_devkitc_n16r8u]` extends `hw_s3_devkitc_n16r8` and only adds `-DBOARD_ANTENNA_EXTERNAL=1`.
- `[hw_esp32_devkit38_wroom32u]` (Module 12) reuses `hw_doit_esp32`'s board and memory settings, but is its own board type in `board.h`.
- No module uses `[hw_doit_esp32]` directly. It stays as the base for genuine 30-pin DOIT boards (LED on GPIO2).
- Only `hw_lilygo_tsim7080g_s3` puts the console on native USB (`sdkconfig.defaults.usb_console` / `ARDUINO_USB_CDC_ON_BOOT=1`), because that board has no USB-UART chip. All other boards use their UART port for the console.

To add a module, add an `[env:mNN_*]` that extends an existing `hw_*`. Copy the `build_flags` pattern (`${hw_x.build_flags} -DBOARD_MODULE_ID=NN ${env.build_flags}`).

To add a hardware type, you need four things:
- a `[hw_*]` section, with settings for both frameworks
- a `sdkconfig/sdkconfig.defaults.<variant>` file
- an `#elif` block in `board.h` that defines every macro listed in its header comment
- a check of the flash type with esptool first

## Commands

```sh
PIO=~/.platformio/penv/Scripts/pio.exe
cd examples/arduino-blink && $PIO run -e m15_esp32s3 -t upload -t monitor

# Identify a board's chip/flash/PSRAM (read-only, resets the board):
~/.platformio/penv/Scripts/python.exe ~/.platformio/packages/tool-esptoolpy/esptool.py --port COMx flash_id
```

All projects live in OneDrive, so each project sets `build_dir = C:/pio-build/<project>` (outside OneDrive). Builds inside OneDrive cause locked or vanishing `.o` files and link errors.

## Hardware gotchas

- **Verify a board's flash type with `esptool flash_id` before adding an octal config.** Octal flash needs `board_build.flash_mode = dout`, plus one setting per framework:
  - ESP-IDF: `CONFIG_ESPTOOLPY_OCT_FLASH=y`
  - Arduino: `board_build.arduino.memory_type = opi_opi`

  A wrong flash mode builds fine but won't boot.
- With QIO selected, ESP-IDF's `CONFIG_ESPTOOLPY_FLASHMODE` shows `"dio"`. That's normal: the bootloader switches the flash to QIO at startup.
- Arduino core 2.x initialises PSRAM only when `-DBOARD_HAS_PSRAM` is set, and needs the right `memory_type`:
  - `qio_opi` for N16R8
  - `opi_opi` for N32R16V
- **Native USB port (USB-Serial-JTAG, VID 303A, PID 1001)**, e.g. Module 19 on COM16. It is normally used only for debugging, but these points apply whenever it is used:
  - Uploads over it auto-reset the chip. But if the board runs firmware that exposes its own USB CDC port (e.g. PID 4001), esptool can't reset it and fails with "Invalid head of packet (0x1B)". Fix: hold BOOT, tap RESET, release BOOT, then upload. Afterwards esptool's hard reset may leave the chip in download mode; press RESET once.
  - When reading a USB-Serial-JTAG port from a script, open it with DTR and RTS deasserted (set `dtr`/`rts` to False **before** `open()`). Asserting them resets the chip into download mode.
  - If firmware puts the console on USB-Serial-JTAG (`sdkconfig.defaults.usb_console`), the UART port shows only the boot ROM output.
- The S3 RGB LED (WS2812) is on GPIO48 for the N16R8 clone boards and GPIO38 for the N32R16V board (verified on hardware).
- **LILYGO T-SIM7080G-S3 (Module 20):** this unit is the "Standard" variant **without** the AXP2101 PMU, so there is no PMU init.
  - Pins in `board.h` come from LILYGO's "T-SIM7080G-S3 Standard PINOUT" diagram (the user has a copy at `OneDrive\Desktop\temp\`).
  - LilyGO's GitHub examples are for the PMU variant and use **different** modem pins (PWR 41 / DTR 42 / RI 3). Don't copy pins from them.
  - The camera uses GPIO35/36/37, which octal PSRAM also needs, so the two can't be used together. Its PSRAM type and size are **not yet verified**; run esptool `flash_id` once the board is connected.
- Don't probe COM3: it's an LG device, not an ESP board.

## Repo

GitHub: `TomHildebrand/esp32-boards`, branch `main`. Consumed as a submodule, e.g. by `TomHildebrand/ESP-IDF-Hello-World` (the project blueprint).
