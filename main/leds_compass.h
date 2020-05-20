#pragma once

#include <driver/gpio.h>
#include <math.h>
#include "leds.h"

//to use with xTaskCreate
void leds_compass_task(void *pvParameters);
