#include "app.h"

#ifdef BOARD_BADGE_NNC2019

#define LEDS_ALARM_REPEAT 20
static bool leds_alarm_state =false;
static bool leds_alarm_repeat=true;
static uint32_t leds_alarm_R=0;
static uint32_t leds_alarm_G=0;
static uint32_t leds_alarm_B=0;
static uint32_t leds_alarm_num=leds.num();
static float    leds_alarm_dim=0.4f;

void leds_effect1_init(uint32_t color=LEDS_STRIP_GREEN, float fDim=0.4f, uint32_t value=10)
{
  leds_alarm_R=LEDS_STRIP_R(color);
  leds_alarm_G=LEDS_STRIP_G(color);
  leds_alarm_B=LEDS_STRIP_B(color);
  leds_alarm_dim=fDim;
  leds_alarm_num=MIN(10, value);
  for(uint32_t i=0; i<leds_alarm_num; i++)
    leds.set_color_rgb(i, leds_alarm_R, leds_alarm_G, leds_alarm_B);
}

void leds_effect1_iter()
{
  if(!leds_alarm_state)
    return;

  for(uint32_t i=0;i<leds.num();i++)
  {
    uint32_t color=leds.get_color_raw(i);
    uint32_t cR=LEDS_STRIP_R(color);
    uint32_t cG=LEDS_STRIP_G(color);
    uint32_t cB=LEDS_STRIP_B(color);
    cR=uint32_t(cR*leds_alarm_dim);
    cG=uint32_t(cG*leds_alarm_dim);
    cB=uint32_t(cB*leds_alarm_dim);
    leds.set_color_rgb(i, cR, cG, cB);
  }

  
  if(!leds_alarm_repeat)
    return;

  static uint32_t period=LEDS_ALARM_REPEAT;
  period--;
  if(period==0)
  {
    period=LEDS_ALARM_REPEAT;
    for(uint32_t i=0; i<leds_alarm_num; i++)
      leds.set_color_rgb(i, leds_alarm_R, leds_alarm_G, leds_alarm_B);
  }

}

void leds_alarm_task(void *pvParameters) {
  leds_effect1_init();
  while (1) {
    leds_effect1_iter();
    leds.update();
    vTaskDelay(10);
  }
  vTaskDelete(NULL); 
}

void leds_alarm_set(bool v, uint32_t color, float fDim, uint32_t value, bool bRepeat)
{
  if(v)
    leds_effect1_init(color, fDim, value);
  else 
    leds.clear();
  leds_alarm_repeat=bRepeat;
  leds_alarm_state=v;
}


#endif