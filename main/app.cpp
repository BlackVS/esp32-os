#include "app.h"

static const char *TAG = __FILE__;

extern I2C_t i2c0;

void self_test(void *pvParameters) {
  LOG_FUNC_BEGIN(TAG)
  const int delay1 = 10;
  const int delay2 = 50; 
  const int LED_OFF = 1;
  const int LED_ON = 0;

  gpio_set_level(PIN_LED_BLUE, LED_OFF);
  while (1) {
    if (touchpad_get_state()==TOUCHPAD_STATE_OFF)
    {
      vTaskDelay(delay2); //must do task delay to not block threads
      continue;
    }
    gpio_set_level(PIN_LED_BLUE, LED_ON);
    vTaskDelay(delay1);
    gpio_set_level(PIN_LED_BLUE, LED_OFF);
    vTaskDelay(delay1);
    gpio_set_level(PIN_LED_BLUE, LED_ON);
    vTaskDelay(delay1);
    gpio_set_level(PIN_LED_BLUE, LED_OFF);
    vTaskDelay(delay2);
  }
  LOG_FUNC_END(TAG)
  vTaskDelete(NULL);
}

extern "C" {
    void app_main(void);
}

void app_main() 
{
  /* Disable buffering on stdin */
  setvbuf(stdin, NULL, _IONBF, 0);

  nvs_initialize();
  vfs_initialize("storage",VFS_MOUNT_PATH);
  console_init();

//  esp_log_level_set("*", ESP_LOG_NONE);
  esp_log_level_set("*", ESP_LOG_DEBUG);
//  esp_log_level_set("*", ESP_LOG_WARN);
  
  print_system_info();

  //init board GPIO
  gpio_init(); // all gpio init staff here - to avoid conflicts between models like MPU, OLED etc

  //tasks init - variables etc
  leds_task_init();
  touchpad_task_init(); //JTAG conflict!!!
  mpu_task_init();  //i2c init moved to gpio_init
  display_init(); //i2c init moved to gpio_init
  WiFi.init();

  leds_alarm_set(false);

  //Touchpad: reads input
  xTaskCreate(&touchpad_task, "Touchpad", 1024 * 4, NULL, 5, NULL);

  //MPU: reads sensora
	xTaskCreate(&mpu_task , "MPU" , 4096, NULL, 5, &mpu_task_handle);

  //Main blue LED" hearbeat on touchpad pressed
  xTaskCreate(&self_test, "SelfTest", 1024 * 2, NULL, 5, NULL);
  
  #ifdef BADGE_LEDS1
  xTaskCreate(&leds_task, "LEDs n ROLL", 1024 * 4, NULL, 5, NULL);
  //xTaskCreate(&oled_heart_sprite_oop_task, "OLED Heart sprite OOP", 4*1024, NULL, 5, NULL);
  #endif

  //Compass+Snow
  #ifdef BADGE_LEDS2
  xTaskCreate(&leds_compass_task, "LEDs compass", 1024 * 4, NULL, 5, NULL);
  //xTaskCreate(&oled_snow_task, "OLED Snow", 4*1024, NULL, 5, NULL);
  #endif
  
  //Space ship - Asteroids
  #ifdef BADGE_ASTEROIDS
  xTaskCreate(&leds_task, "LEDs n ROLL", 1024 * 4, NULL, 5, NULL);
  //xTaskCreate(&oled_ship_task, "OLED Ship", 4*1024, NULL, 5, NULL);
  #endif 

  #ifdef BADGE_CORONA
  //Corona game
  xTaskCreate(&leds_alarm_task, "LEDS Alarm", 1024 * 4, NULL, 5, NULL);
  //xTaskCreate(&oled_corona_fighter_task, "CORONA Fighter", 4*1024, NULL, 5, NULL);
  #endif

  //OLED, choose one of:
  //xTaskCreate(&oled_logo_task, "OLED", 1024 * 4, NULL, 5, NULL);
  //xTaskCreate(&oled_cross_sprite_task, "OLED Cross sprite", OLED_TASK_STACK, NULL, 5, NULL);
  //xTaskCreate(&oled_space_ship_task, "OLED Space Ship", 4*1024, NULL, 5, NULL);

  //xTaskCreatePinnedToCore(&console_task, "OS Shell", 1024 * 6, NULL, 5, NULL, 1);
  console_task(NULL);
}
