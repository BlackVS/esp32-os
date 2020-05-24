#pragma once

#define CONFIG_STORE_HISTORY 1

//#define MAX(x, y) (((x) > (y)) ? (x) : (y))
//#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#include <stdio.h>
#include <string>
#include <vector>
#include <cstdint>

#include "log.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_tls.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_ota_ops.h"

///// netif not supports C++ compiler yet
#ifdef __cplusplus
extern "C" {
#endif

#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_sntp.h"

//#include "lwip/apps/sntp.h"
//#include "lwip/err.h"

#ifdef __cplusplus
}
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#include "driver/gpio.h"
#include "driver/i2c.h"

#include "sdkconfig.h"

//general toolkits
#include "I2Cbus.hpp"
#include "lcdgfx.h"

//hardware
#include "vfs.h"
#include "mpu_task.h"
#include "touch_button.h"
#include "led_strip.h"
#include "nvs_settings.h"
#include "wifi.h"

//resource
#include "resources.h"

//boards
#include "boards.h"


//
#include "utils.h"


//tasks
#include "leds.h"
#include "leds_compass.h"
#include "console.h"
#include "oled.h"

    