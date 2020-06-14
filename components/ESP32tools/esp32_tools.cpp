#include "esp32_tools.h"

//__uint32_t millis(void)
//{
//   struct timespec ts;
//   clock_gettime(CLOCK_MONOTONIC, &ts);
//   return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
//};

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/gpio.h>
#include "esp_log.h"
#include <ctime>

uint32_t millis(void)
{
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

void delay(int ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

void print_time(void)
{
  time_t now;
  char strftime_buf[64];
  struct tm timeinfo;

  time(&now);
  setenv("TZ", "GMT0", 1);
  tzset();

  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
  printf("The current date/time is: %s GMT0\n", strftime_buf);
}


// uint8_t lcd_pgmReadByte(const void *ptr)
// {
//     return *((const uint8_t *)ptr);
// }

//static inline uint8_t pgm_read_byte(const void *ptr) { return *((const uint8_t *)ptr); }

int random(int v)
{
    return rand() % v;
}

int random(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

int  gpioRead(int pin)
{
    return gpio_get_level(static_cast<gpio_num_t>(pin));
}

int  adcRead(int pin)
{
    // TODO: Not implemented
    assert(0);
    return 0;
}