#include "app.h"

#ifdef BOARD_BADGE_NNC2019

#include "board_nnc2019.h"
#define randrange(a,b) lcd_random(a,b) 

typedef struct {
  uint32_t r1, g1, b1, pos1, dir1;
  uint32_t r2, g2, b2, pos2, dir2;
  float f_dim;
} EFFECT0_DATA;

void leds_effect0_rand(EFFECT0_DATA& data){
  data.b1=randrange(0,255);
  data.g1=randrange(0,255);
  data.r1=randrange(0,255);
  data.dir1 = rand()> (RAND_MAX/2) ? 1 : -1;
  data.b2=randrange(0,255);
  data.g2=randrange(0,255);
  data.r2=randrange(0,255);
  data.dir2 = rand()> (RAND_MAX/2) ? 1 : -1;
}

void leds_effect0_init(EFFECT0_DATA& data, uint32_t clr1=LEDS_STRIP_BLUE, uint32_t clr2=LEDS_STRIP_YELLOW)
{
  data.b1=LEDS_STRIP_B(clr1);
  data.g1=LEDS_STRIP_G(clr1);
  data.r1=LEDS_STRIP_R(clr1);
  data.dir1 = rand()> (RAND_MAX/2) ? 1 : -1;
  data.b2=LEDS_STRIP_B(clr2);
  data.g2=LEDS_STRIP_G(clr2);
  data.r2=LEDS_STRIP_R(clr2);
  data.dir2 = rand()> (RAND_MAX/2) ? 1 : -1;
  data.pos1=0;
  data.pos2=leds.num()/2;
  data.f_dim=0.4f;
}


void leds_effect0_dim(float f)
{
  for(int i=0; i<leds.num(); i++){
    uint32_t c=leds.get_color_raw(i);
    uint32_t r=(uint32_t)(LEDS_STRIP_R(c)*f);
    uint32_t g=(uint32_t)(LEDS_STRIP_G(c)*f);
    uint32_t b=(uint32_t)(LEDS_STRIP_B(c)*f);
    leds.set_color_rgb(i,r,g,b);
  }
}

void leds_effect0_draw(EFFECT0_DATA& data)
{
  leds.clear();
  uint32_t r,g,b,c,p;
  uint32_t rc,gc,bc;
  rc = data.r1;
  gc = data.g1;
  bc = data.b1;
  p=data.pos1;
  uint32_t num_leds=leds.num();

  for(int i=0;i<num_leds;i++,p=(p-data.dir1+num_leds)%num_leds)
  {
    c=leds.get_color_raw(p);
    r=LEDS_STRIP_R(c)+rc;
    g=LEDS_STRIP_G(c)+gc;
    b=LEDS_STRIP_B(c)+bc;
    leds.set_color_rgb(p,r,g,b);
    rc=(uint32_t)(rc*data.f_dim);
    gc=(uint32_t)(gc*data.f_dim);
    bc=(uint32_t)(bc*data.f_dim);
  }
  rc = data.r2;
  gc = data.g2;
  bc = data.b2;
  p=data.pos2;
  for(int i=0;i<num_leds;i++,p=(p-data.dir2+num_leds)%num_leds)
  {
    c=leds.get_color_raw(p);
    r=LEDS_STRIP_R(c)+rc;
    g=LEDS_STRIP_G(c)+gc;
    b=LEDS_STRIP_B(c)+bc;
    leds.set_color_rgb(p,r,g,b);
    rc=(uint32_t)(rc*data.f_dim);
    gc=(uint32_t)(gc*data.f_dim);
    bc=(uint32_t)(bc*data.f_dim);
  }
}

void leds_effect0_iter(EFFECT0_DATA& data)
{
  uint32_t num_leds=leds.num();
  data.pos1=(data.pos1+data.dir1+num_leds)%num_leds;
  data.pos2=(data.pos2+data.dir2+num_leds)%num_leds;
}

 void leds_task(void *pvParameters) 
 {
  EFFECT0_DATA effect_data;
  leds_effect0_init(effect_data);
  float f=1.0f;
  while (1) {
    leds_effect0_draw(effect_data);
    leds_effect0_iter(effect_data);
    leds_effect0_dim(f);
    if (touchpad.get_state()==TOUCHPAD_STATE_ON)
      f=f*0.8f;
    else
      f=f*1.2f;
    if(f<0.01f) {
      f=0.01f;
      leds_effect0_rand(effect_data);
    }
    if(f>0.95f){
      f=1.0f;
    }
    leds.update();
    vTaskDelay(10);
  }
   vTaskDelete(NULL); 
 }



#endif