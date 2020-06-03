#pragma once


typedef enum {
    TFT_UNKNOWN=-1,
    TFT_AUTO   =0,
    TFT_ST7735_128x128x16,
    TFT_ST7735_128x160x16,
    TFT_ST7735_M5STICKC,
    TFT_SSD1306_128x64x1
} TFT_TYPE;
 
typedef enum {
    TFT_ROTATION_DEFAULT=0,
    TFT_ROTATION_0  =0,
    TFT_ROTATION_90 =1,
    TFT_ROTATION_180=2,
    TFT_ROTATION_270=3,
    //
    TFT_ROTATION_MAX=4
} TFT_ROTATION;

class ESP32_TFT_Notifications
{
    public:
        virtual void on_rotation_changed()=0;
};

inline void tft_delay(unsigned long ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}