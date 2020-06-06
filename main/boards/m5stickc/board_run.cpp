#include "app.h"

#include "oled.h"

#define TAG "TTGO"


void board_run() 
{
  ESP_LOGD(TAG, "%s", __FUNCTION__);

  #ifdef DEMO_HEARTS
  xTaskCreate(&oled_heart_sprite_oop_task, "OLED Heart sprite OOP", 4*1024, NULL, 5, NULL);
  #endif

  #ifdef DEMO_SNOW
  xTaskCreate(&oled_snow_task, "OLED Snow", 4*1024, NULL, 5, NULL);
  #endif
  
}

