#include "esp32_tools.h"

//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
#include "esp_log.h"
#include "boards.h"
#include "esp_spi_flash.h"
//#include <ctime>

void print_system_info(void)
{
  printf( LOG_COLOR_I
          "\n\n\n"
          "==============================\n"
          " Esp32 OS v%s by BlackVS/VVS \n"
          "==============================\n"
          , FIRMWARE_VERSION
         );
  
  /* Print chip information */
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  printf(" Chip        : ESP32 rev. %i \n", chip_info.revision);
  //printf(" IDF version : %s\n", esp_get_idf_version());
  printf(" CPU cores   : %d\n"
         " Wireless    : WiFi%s%s\n", chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
  printf(" Flash       : %dMB %s\n", spi_flash_get_chip_size() / (1024 * 1024),
          (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
  printf(" Free heap   : %dKB \n", esp_get_free_heap_size()/1024);

  uint8_t base_mac[6];
  esp_efuse_mac_get_default(base_mac);
  printf(" Default MAC : %02x:%02x:%02x:%02x:%02x:%02x\n", base_mac[0], base_mac[1], base_mac[2], base_mac[3], base_mac[4], base_mac[5]);

  print_time();
}