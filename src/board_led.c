#include "board_led.h"
#include "board.h"

#if BOARD_PIN_LED < 0
/* ---- no user LED ----------------------------------------------------------- */
esp_err_t board_led_init(void) { return ESP_OK; }
void board_led_set_rgb(uint8_t r, uint8_t g, uint8_t b) { (void)r; (void)g; (void)b; }

#elif defined(ARDUINO)
/* ---- Arduino: core functions ------------------------------------------------ */
#include "esp32-hal-gpio.h"
#include "esp32-hal-rgb-led.h"

esp_err_t board_led_init(void)
{
#if !BOARD_LED_IS_RGB
    pinMode(BOARD_PIN_LED, OUTPUT);
#endif
    board_led_off();
    return ESP_OK;
}

void board_led_set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
#if BOARD_LED_IS_RGB
    neopixelWrite(BOARD_PIN_LED, r, g, b);           // WS2812, sends GRB order itself
#else
    digitalWrite(BOARD_PIN_LED, (r | g | b) ? HIGH : LOW);
#endif
}

#elif BOARD_LED_IS_RGB
/* ---- ESP-IDF: WS2812 RGB LED via RMT ----------------------------------------- */
#include "driver/rmt_tx.h"

#define WS2812_RESOLUTION_HZ  (10 * 1000 * 1000)    // 10 MHz -> 0.1 us per tick

static rmt_channel_handle_t s_chan;
static rmt_encoder_handle_t s_encoder;

esp_err_t board_led_init(void)
{
    rmt_tx_channel_config_t chan_cfg = {
        .gpio_num          = BOARD_PIN_LED,
        .clk_src           = RMT_CLK_SRC_DEFAULT,
        .resolution_hz     = WS2812_RESOLUTION_HZ,
        .mem_block_symbols = 64,
        .trans_queue_depth = 4,
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&chan_cfg, &s_chan));

    // WS2812 bit timing: 0 = 0.3 us high + 0.9 us low, 1 = 0.9 us high + 0.3 us low
    rmt_bytes_encoder_config_t enc_cfg = {
        .bit0 = { .level0 = 1, .duration0 = 3, .level1 = 0, .duration1 = 9 },
        .bit1 = { .level0 = 1, .duration0 = 9, .level1 = 0, .duration1 = 3 },
        .flags.msb_first = 1,
    };
    ESP_ERROR_CHECK(rmt_new_bytes_encoder(&enc_cfg, &s_encoder));
    ESP_ERROR_CHECK(rmt_enable(s_chan));
    board_led_off();
    return ESP_OK;
}

void board_led_set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    const uint8_t grb[3] = { g, r, b };              // WS2812 expects GRB order
    rmt_transmit_config_t tx_cfg = { .loop_count = 0 };
    rmt_transmit(s_chan, s_encoder, grb, sizeof(grb), &tx_cfg);
    rmt_tx_wait_all_done(s_chan, 100);               // >50 us idle low = latch
}

#else
/* ---- ESP-IDF: plain single-colour LED --------------------------------------- */
#include "driver/gpio.h"

esp_err_t board_led_init(void)
{
    gpio_reset_pin(BOARD_PIN_LED);
    gpio_set_direction(BOARD_PIN_LED, GPIO_MODE_OUTPUT);
    board_led_off();
    return ESP_OK;
}

void board_led_set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    gpio_set_level(BOARD_PIN_LED, (r | g | b) ? 1 : 0);
}
#endif

void board_led_off(void)
{
    board_led_set_rgb(0, 0, 0);
}
