#include "app.h"

//__uint32_t millis(void)
//{
//   struct timespec ts;
//   clock_gettime(CLOCK_MONOTONIC, &ts);
//   return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
//};

void delay(int ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

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

  mac_t base_mac;
  esp_efuse_mac_get_default(base_mac.buf());
  printf(" Default MAC : %s\n", mac2str(base_mac).c_str());

  print_time();
}