#pragma once
#include <cstdint>

uint32_t millis(void);
void delay(int ms);
void print_time(void);
int  random(int v);
int  random(int min, int max);
int  gpioRead(int pin);
int  adcRead(int pin);

#define randrange(a,b) random(a,b) 

#define SWAP(a, b) { typeof(a) temp = a; a = b; b = temp; }