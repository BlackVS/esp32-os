#pragma once

#include <driver/gpio.h>
#include <math.h>
#include "led_strip.h"

// //to use with xTaskCreate
void leds_task(void *pvParameters);
void leds_alarm_task(void *pvParameters);
void leds_alarm_set(bool v, uint32_t color=LEDS_STRIP_RED, float fDim=0.4f, uint32_t leds=10, bool bRepeat=true);
void leds_compass_task(void *pvParameters);
