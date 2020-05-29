#include "app.h"

#ifdef BOARD_BADGE_NNC2019

void angle2b(float a, int* i0, int* b0, int *i1, int *b1){
      int iangle = ((int)a+360)%360;

      int iled0 = iangle/36;
      int iled1 = (iled0 + 1) % leds.num();

      int r = iangle % 36;
      int bled0 = (35-r) * 4;
      int bled1 = r*4;
      *i0=iled0;
      *i1=iled1;
      *b0=bled0;
      *b1=bled1;
}

void leds_compass_task(void *pvParameters) {
  while (1) {
    CMPUData data;
    if(MPU_get(data)==0){
      //printf("%7.3f %7.3f %7.3f --- ", data.angles[0], data.angles[1], data.angles[2] );
      //printf("%7.3f %7.3f %7.3f --- ", data.accLinear[0], data.accLinear[1], data.accLinear[2] );
      //printf("%7.3f %7.3f %7.3f --- ", data.accTotal[0], data.accTotal[1], data.accTotal[2] );
      //printf("%7.3f %7.3f %7.3f\n", data.gyro[0], data.gyro[1], data.gyro[2] );
      //printf("\n");
      leds.clear();

      //float x = data.accTotal[0];
      //float y = data.accTotal[1];

      //float r = sqrtf(x*x+y*y);
      //float alpha = alpha= atan2f(y,x);
      //printf("%9.4f\n",alpha);
      int i0, i1, b0, b1;

      
      // angle2b(360-data.pitch, &i0, &b0, &i1, &b1);
      // leds_add_color_rgb(i0, b0, 0, 0);
      // leds_add_color_rgb(i1, b1, 0, 0);
      
      //angle2b(data.roll, &i0, &b0, &i1, &b1);
      //leds_add_color_rgb(i0, 0, b0, 0);
      //leds_add_color_rgb(i1, 0, b1, 0);

      angle2b(-data.yaw, &i0, &b0, &i1, &b1);
      leds.add_color_rgb(i0, 0, 0, b0);
      leds.add_color_rgb(i1, 0, 0, b1);

      //printf("%i %i\n",iangle, iled0);
      leds.update();
    }
    vTaskDelay(10);
  }
  vTaskDelete(NULL); 
}


#endif