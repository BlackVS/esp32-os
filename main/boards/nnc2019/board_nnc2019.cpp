#include "app.h"

#ifdef BOARD_BADGE_NNC2019

static const char *TAG = __FILE__;
Board_NNC2019 board=Board_NNC2019();
//////////////////////////////////////////////////////////////////////////

/// Board compomnents:
DISPLAY_TYPE display(-1, i2c0, OLED_I2C_ADDR );
CBlink       blue_led(NNC2019::config::PIN_LED_BLUE);
CLEDStrip    leds(NNC2019::config::LEDS_NUM, NNC2019::config::LEDS_RMT_CHANNEL, NNC2019::config::LEDS_PIN );
#ifndef NNC2019_JTAG_ENABLED
CTouchButton touchpad(NNC2019::config::PIN_TOUCH);
#else
CTouchButtonDummy touchpad;
#endif




void self_test_task(void *pvParameters) 
{
    LOG_FUNC_BEGIN(TAG)
    const int delay1 = 10;
    const int delay2 = 50; 

    blue_led.off();

    while (1) 
    {
        if (touchpad.get_state()==TOUCHPAD_STATE_OFF)
        {
            vTaskDelay(delay2); //must do task delay to not block threads
            continue;
        }
        blue_led.on();
        vTaskDelay(delay1);
        blue_led.off();
        vTaskDelay(delay1);
        blue_led.on();
        vTaskDelay(delay1);
        blue_led.off();
        vTaskDelay(delay2);
    }
    LOG_FUNC_END(TAG)
    vTaskDelete(NULL);
}


Board_NNC2019::Board_NNC2019()
{
    //delay(100); for breakpoints only
}


void Board_NNC2019::init() 
{
    // Initialize I2C on port 0 using I2Cbus interface
    i2c0.setup(MPU_I2C_SDA_IO, MPU_I2C_SCL_IO, MPU_I2C_FREQ_HZ);
    i2c0.begin();
    //
    WiFi.init();
    mpu_task_init();  //i2c init moved to gpio_init
    blue_led.init();
    leds.init();
    touchpad.init();

    //Display init
    ESP_LOGD(__FUNCTION__, "Starting...");
    display.begin(); // will raise error on i2c init - it is normal
    display.setFixedFont(ssd1306xled_font6x8);
    display.clear();
    uint32_t x=16;
    uint32_t y=8;
    display.printFixed(x, y+=8, "===============", STYLE_NORMAL);
    display.printFixed(x, y+=8, " ESP OS by VVS ", STYLE_BOLD);
    display.printFixed(x, y+=8, "===============", STYLE_NORMAL);
    display.printFixed(x, y+=8, "  version " FIRMWARE_VERSION, STYLE_ITALIC);
    delay(2000);


    ESP_LOGD(__FUNCTION__, "Successfully started...");
}


void Board_NNC2019::run() 
{
  ESP_LOGD(TAG, "%s", __FUNCTION__);
  //Main blue LED" hearbeat on touchpad pressed
  xTaskCreate(&self_test_task, "SelfTest", 1024 * 2, NULL, 5, NULL);
  touchpad.run();

  //MPU: reads sensora
  xTaskCreate(&mpu_task , "MPU" , 4096, NULL, 5, &mpu_task_handle);

  #ifdef BADGE_LEDS1
  xTaskCreate(&leds_task, "LEDs n ROLL", 1024 * 4, NULL, 5, NULL);
  xTaskCreate(&oled_heart_sprite_oop_task, "OLED Heart sprite OOP", 4*1024, NULL, 5, NULL);
  #endif

  //Compass+Snow
  #ifdef BADGE_LEDS2
  xTaskCreate(&leds_compass_task, "LEDs compass", 1024 * 4, NULL, 5, NULL);
  xTaskCreate(&oled_snow_task, "OLED Snow", 4*1024, NULL, 5, NULL);
  #endif
  
  //Space ship - Asteroids
  #ifdef BADGE_ASTEROIDS
  xTaskCreate(&leds_task, "LEDs n ROLL", 1024 * 4, NULL, 5, NULL);
  xTaskCreate(&oled_ship_task, "OLED Ship", 4*1024, NULL, 5, NULL);
  #endif 

  #ifdef BADGE_CORONA
  //Corona game
  xTaskCreate(&leds_alarm_task, "LEDS Alarm", 1024 * 4, NULL, 5, NULL);
  xTaskCreate(&oled_corona_fighter_task, "CORONA Fighter", 4*1024, NULL, 5, NULL);
  #endif


}

#endif