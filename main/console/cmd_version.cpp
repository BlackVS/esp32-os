#include "app.h"

void register_cmd_version() {
  const esp_console_cmd_t cmd = {
      .command = "version",
      .help = "Get version of chip and SDK",
      .hint = NULL,
      .func = &cmd_version,
      .argtable = NULL
  };
  ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/* 'version' command */
int cmd_version(int argc, char **argv) 
{
  esp_chip_info_t info;
  esp_chip_info(&info);

  printf("Firmware Version: %s\r\n", FIRMWARE_VERSION);
  printf("Chip info:\r\n");
  printf("\tmodel:%s\r\n", info.model == CHIP_ESP32 ? "ESP32" : "Unknown");
  printf("\tcores:%d\r\n", info.cores);
  printf("\tfeature:%s%s%s%s%d%s\r\n",
         info.features & CHIP_FEATURE_WIFI_BGN ? "/802.11bgn" : "",
         info.features & CHIP_FEATURE_BLE ? "/BLE" : "",
         info.features & CHIP_FEATURE_BT ? "/BT" : "",
         info.features & CHIP_FEATURE_EMB_FLASH ? "/Embedded-Flash:"
                                                : "/External-Flash:",
         spi_flash_get_chip_size() / (1024 * 1024), " MB");
  printf("\trevision number:%d\r\n", info.revision);
  return 0;
}
