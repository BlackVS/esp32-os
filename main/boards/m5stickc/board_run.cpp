#include "app.h"

#include "oled.h"

#define TAG "M5STICKC"


void board_run() 
{
  ESP_LOGD(TAG, "%s", __FUNCTION__);

  #ifdef DEMO_HEART
  xTaskCreate(&oled_heart_sprite_oop_task, "OLED Heart sprite OOP", 4*1024, NULL, 5, NULL);
  #endif

  //Compass+Snow
  #ifdef DEMO_SNOW
  xTaskCreate(&oled_snow_task, "OLED Snow", 4*1024, NULL, 5, NULL);
  #endif
  
  //Space ship - Asteroids
  #ifdef DEMO_SHIP
  xTaskCreate(&oled_ship_task, "OLED Ship", 4*1024, NULL, 5, NULL);
  #endif 

  #ifdef DEMO_GAME_CORONA
  //Corona game
  xTaskCreate(&oled_corona_fighter_task, "CORONA Fighter", 4*1024, NULL, 5, NULL);
  #endif


}

