#pragma once


#include <cstdint>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/** swaps content of a and b variables of type type */
#define ssd1306_swap_data(a, b, type)  { type t = a; a = b; b = t; }

/** Constant corresponds to low level of gpio pin */
#define LCD_LOW 0
/** Constant corresponds to high level of gpio pin */
#define LCD_HIGH 1
// /** Constant corresponds to input mode of gpio */

#define pgm_read_byte lcd_pgmReadByte
#define random lcd_random

int  lcd_gpioRead(int pin)
{
    return gpio_get_level(static_cast<gpio_num_t>(pin));
}

uint32_t lcd_millis(void)
{
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

void lcd_delay(unsigned long ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

int  lcd_adcRead(int pin)
{
    // TODO: Not implemented
    assert(0);
    return 0;
}

uint8_t lcd_pgmReadByte(const void *ptr)
{
    return *((const uint8_t *)ptr);
}

int lcd_random(int v)
{
    return rand() % v;
}

int lcd_random(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

