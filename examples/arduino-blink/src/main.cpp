// Arduino LED test on any module from boards.ini: blinks a plain LED, cycles the
// colours of a WS2812 RGB LED, or just prints a counter if the board has no LED.
#include <Arduino.h>
#include "board.h"
#include "board_led.h"

static const struct { uint8_t r, g, b; const char *name; } steps[] = {
    { 32,  0,  0, "red"   }, {  0,  0,  0, "off" },
    {  0, 32,  0, "green" }, {  0,  0,  0, "off" },
    {  0,  0, 32, "blue"  }, {  0,  0,  0, "off" },
    { 24, 24, 24, "white" }, {  0,  0,  0, "off" },
};

void setup()
{
    Serial.begin(115200);
    delay(1000);    // give USB CDC / the serial monitor time to attach
    Serial.printf("Hello world (Arduino) from Module %d (%s)!\n", BOARD_MODULE_ID, BOARD_NAME);
    Serial.printf("Flash %d MB, PSRAM %d MB (board.h); found PSRAM: %u bytes\n",
                  BOARD_FLASH_MB, BOARD_PSRAM_MB, (unsigned)ESP.getPsramSize());
    board_led_init();
}

void loop()
{
    static int i = 0, counter = 0;
    board_led_set_rgb(steps[i].r, steps[i].g, steps[i].b);
    Serial.printf("Hello from Arduino! (%d) LED %s\n", counter++,
                  BOARD_PIN_LED < 0 ? "-" : steps[i].name);
    i = (i + 1) % (int)(sizeof(steps) / sizeof(steps[0]));
    delay(500);
}
