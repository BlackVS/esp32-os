#include "app.h"

//static const char *TAG = __FILE__;


extern "C" {
    void app_main(void);
}

void app_main() 
{
  /* Disable buffering on stdin */
  setvbuf(stdin, NULL, _IONBF, 0);
  srand(time(NULL));

  //General for all ESP32 boards
  nvs_initialize();
  vfs_initialize("storage",VFS_MOUNT_PATH);
  console_init();

//  esp_log_level_set("*", ESP_LOG_NONE);
  esp_log_level_set("*", ESP_LOG_DEBUG);
//  esp_log_level_set("*", ESP_LOG_WARN);
  
  print_system_info();

  //init board GPIO
  gpio_init(); // all gpio init staff here - to avoid conflicts between models like MPU, OLED etc

  //specific of board
  board.init();

  //tasks init - variables etc
  #ifdef BOARD_M5STICKC
  display_init(); //i2c init moved to gpio_init
  #endif

  #ifdef BOARD_BADGE_NNC2019
  //leds_task_init();
  touchpad_task_init(); //JTAG conflict!!!
  mpu_task_init();  //i2c init moved to gpio_init
  #endif

  WiFi.init();


  #ifdef BOARD_BADGE_NNC2019

  //leds_alarm_set(false);
  //Touchpad: reads input
  xTaskCreate(&touchpad_task, "Touchpad", 1024 * 4, NULL, 5, NULL);

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

  #endif 
  //OLED, choose one of:
  //xTaskCreate(&oled_logo_task, "OLED", 1024 * 4, NULL, 5, NULL);
  //xTaskCreate(&oled_cross_sprite_task, "OLED Cross sprite", OLED_TASK_STACK, NULL, 5, NULL);
  //xTaskCreate(&oled_space_ship_task, "OLED Space Ship", 4*1024, NULL, 5, NULL);

  //xTaskCreatePinnedToCore(&console_task, "OS Shell", 1024 * 6, NULL, 5, NULL, 1);
  board.run();
  console_task(NULL);
}
