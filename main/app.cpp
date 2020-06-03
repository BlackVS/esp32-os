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

  //General for all ESP32 boards initialization
  nvs_initialize();
  vfs_initialize("storage", VFS_MOUNT_PATH);

  console_init();

//  esp_log_level_set("*", ESP_LOG_NONE);
//  esp_log_level_set("*", ESP_LOG_DEBUG);
  esp_log_level_set("*", ESP_LOG_WARN);
  
  print_system_info();

  //initialize board specific hardware
  board.init();

  //run board specific threads
  board.run();

  //common for all boards tasks
  console_task(NULL);

}
