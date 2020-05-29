#pragma once

#include "lcd_hal/io.h"
#include "v2/lcd/lcd_common.h"
#include "v2/lcd/base/display.h"

#include "esp32_tft.h"

typedef  NanoDisplayOps<NanoDisplayOps16<ESP32_SPI_TFT>, ESP32_SPI_TFT> ESP32_Display_Engine;

class ESP32_Display: public ESP32_Display_Engine 
{
public:
    ESP32_Display( ESP32_SPI_TFT& tft )
        : ESP32_Display_Engine(tft),
          m_tft(tft)
        {
            this->m_w=tft.width();
            this->m_h=tft.height();
        }

    void begin() { m_tft.begin(); }
    void end()  { m_tft.end(); }

private:
    ESP32_SPI_TFT m_tft;
};

