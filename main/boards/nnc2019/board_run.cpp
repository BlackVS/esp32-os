#include "app.h"



#define TAG "NNC2019"


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


void board_run() 
{
  ESP_LOGD(TAG, "%s", __FUNCTION__);
  //Main blue LED" hearbeat on touchpad pressed
  xTaskCreate(&self_test_task, "SelfTest", 1024 * 2, NULL, 5, NULL);
  touchpad.run();

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

