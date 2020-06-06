#pragma once

#include <cstdint>
#include <driver/rmt.h>
#include <driver/gpio.h>
#include "freertos/FreeRTOS.h"

// Configure these based on your project needs ********
//#define LED_RMT_TX_CHANNEL RMT_CHANNEL_0
//#define LED_RMT_TX_GPIO PIN_WS2812
// ****************************************************

//#define BITS_PER_LED_CMD 24
//#define LED_BUFFER_ITEMS ((NUM_LEDS * BITS_PER_LED_CMD))

// These values are determined by measuring pulse timing with logic analyzer and
// adjusting to match datasheet.
#define T0H 14 // 0 bit high time
#define T1H 52 // 1 bit high time
#define TL 52  // low time for either bit

#define LEDS_STRIP_BITS_PER_LED 24
#define LEDS_STRIP_MAX_LEDS 16
#define LEDS_STRIP_MAX_DATABUFLEN (LEDS_STRIP_MAX_LEDS * LEDS_STRIP_BITS_PER_LED)

#define LEDS_STRIP_WHITE  0xFFFFFF
#define LEDS_STRIP_GREEN  0xFF0000
#define LEDS_STRIP_RED    0x00FF00
#define LEDS_STRIP_BLUE   0x0000FF
#define LEDS_STRIP_YELLOW 0xFFFF00

#define LEDS_STRIP_RGB(r,g,b)  (uint32_t)(((uint32_t)g<<16)|((uint32_t)r<<8)|b);
#define LEDS_STRIP_B(c)  ((c)&0xff)
#define LEDS_STRIP_R(c)  (((c)>>8)&0xff)
#define LEDS_STRIP_G(c)  (((c)>>16)&0xff)

class CLEDStrip 
{

    private:
        rmt_item32_t    m_data_buffer[LEDS_STRIP_MAX_DATABUFLEN];
        uint32_t        m_data_buffer_items = {0};
        uint32_t        m_leds[LEDS_STRIP_MAX_LEDS];    //full range BGR states
        uint32_t        m_leds_raw[LEDS_STRIP_MAX_LEDS];//=m_leds*brightness i.e. raw state
        uint32_t        m_leds_num          = {0};
        uint32_t        m_bits_per_led      = {LEDS_STRIP_BITS_PER_LED}; 
        uint32_t        m_leds_brightness   = {100};
        //gpio
        rmt_channel_t   m_rmt_channel       = {RMT_CHANNEL_0};
        gpio_num_t      m_gpio_num          = {GPIO_NUM_NC};


    protected:
        void ws2812_write_leds(void);
        void setup_rmt_data_buffer(void);

    public:
        CLEDStrip(uint32_t num_leds, rmt_channel_t channel, gpio_num_t gpio_num);
        ~CLEDStrip(){}
        void init();

    //
    uint32_t num() { return m_leds_num; }

    //
    void clear();
    void update();
    void set_color_raw(uint32_t index, uint32_t color);
    void set_color_rgb(uint32_t index, uint32_t r, uint32_t g, uint32_t b);
    void add_color_rgb(uint32_t index, uint32_t ra, uint32_t ga, uint32_t ba);
    uint32_t get_color_raw(uint32_t index);

};

// This structure is used for indicating what the colors of each LED should be
// set to. There is a 32bit value for each LED. Only the lower 3 bytes are used
// and they hold the Red (byte 2), Green (byte 1), and Blue (byte 0) values to
// be set.

// struct led_state {
//   uint32_t leds[NUM_LEDS];
// };


// Update the LEDs to the new state. Call as needed.
// This function will block the current task until the RMT peripheral is
// finished sending the entire sequence.
// void ws2812_write_leds(struct led_state new_state);

