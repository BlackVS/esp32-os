#include "app.h"

// This is the buffer which the hw peripheral will access while pulsing the
// output pin
// rmt_item32_t led_data_buffer[LED_BUFFER_ITEMS];

// static void setup_rmt_data_buffer(struct led_state new_state);

        // /* Init LED */
        // gpio_pad_select_gpio(BLINK_GPIO);
        // /* Set the GPIO as a push/pull output */
        // gpio_set_direction(BLINK_GPIO, GPIO_MODE_INPUT_OUTPUT);
        // //i2c should be already inited

CLEDStrip::CLEDStrip(uint32_t num_leds, rmt_channel_t channel, gpio_num_t gpio_num) 
{
  m_leds_num   =MIN(num_leds, LEDS_STRIP_MAX_LEDS);
  m_rmt_channel=channel;
  m_gpio_num   =gpio_num;
  m_data_buffer_items = MIN(m_leds_num * m_bits_per_led, LEDS_STRIP_MAX_DATABUFLEN);
}

void CLEDStrip::init(void) 
{
  //init WS2812
  rmt_config_t config;
  config.rmt_mode = RMT_MODE_TX;
  config.channel  = m_rmt_channel;
  config.gpio_num = m_gpio_num;
  config.mem_block_num = 3;
  config.tx_config.loop_en = false;
  config.tx_config.carrier_en = false;
  config.tx_config.idle_output_en = true;
  config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
  config.clk_div = 2;

  ESP_ERROR_CHECK(rmt_config(&config));
  ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

  //
  clear();
  update();

}


void CLEDStrip::ws2812_write_leds() 
{
  setup_rmt_data_buffer();
  ESP_ERROR_CHECK(rmt_write_items(m_rmt_channel, m_data_buffer, m_data_buffer_items, false));
  ESP_ERROR_CHECK(rmt_wait_tx_done(m_rmt_channel, portMAX_DELAY));
}

void CLEDStrip::setup_rmt_data_buffer() 
{
  for (uint32_t led = 0; led < m_leds_num; led++) 
  {
    uint32_t bits_to_send = m_leds_raw[led];
    uint32_t mask = 1 << (m_bits_per_led - 1);
    for (uint32_t bit = 0; bit < m_bits_per_led; bit++) 
    {
      uint32_t bit_is_set = bits_to_send & mask;
      m_data_buffer[led * m_bits_per_led + bit] =
          bit_is_set ? (rmt_item32_t){{{T1H, 1, TL, 0}}}
                     : (rmt_item32_t){{{T0H, 1, TL, 0}}};
      mask >>= 1;
    }
  }
}


void CLEDStrip::clear(void)
{
   for (int i = 0; i < m_leds_num; i++) 
     set_color_raw(i, 0);
}

void CLEDStrip::update(void)
{
  uint32_t bmax=m_leds_brightness;
  uint32_t r, g, b;
  for(int i=0; i<m_leds_num; i++)
  {
      uint32_t c = m_leds[i];
      r=(LEDS_STRIP_R(c)*bmax)/255;
      g=(LEDS_STRIP_G(c)*bmax)/255;
      b=(LEDS_STRIP_B(c)*bmax)/255;
      m_leds_raw[i]=LEDS_STRIP_RGB(r,g,b);
  }
  ws2812_write_leds(); 
}

void CLEDStrip::set_color_raw(uint32_t index, uint32_t color) 
{
  if(index<m_leds_num)
    m_leds[index] = color;
}

uint32_t CLEDStrip::get_color_raw(uint32_t index) 
{
  if(index<m_leds_num)
    return m_leds[index];
  return 0;
}

void CLEDStrip::set_color_rgb(uint32_t index, uint32_t r, uint32_t g, uint32_t b) 
{
  if(index<m_leds_num)
  {
    r=MIN(255, r);
    g=MIN(255, g);
    b=MIN(255, b);
    m_leds[index] = LEDS_STRIP_RGB(r,g,b);
  }
}

void CLEDStrip::add_color_rgb(uint32_t index, uint32_t ra, uint32_t ga, uint32_t ba) 
{
  if(index<m_leds_num)
  {
    uint32_t r, g, b, c;
    c=get_color_raw(index);
    r=LEDS_STRIP_R(c)+ra;
    g=LEDS_STRIP_G(c)+ga;
    b=LEDS_STRIP_B(c)+ba;
    set_color_rgb(index,r,g,b);
  }
}

