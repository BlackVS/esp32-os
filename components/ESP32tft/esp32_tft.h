#pragma once

#include "lcdgfx.h"
#include "esp32_tft_common.h"

//OPS = NanoDisplayOps1..NanoDisplayOps16
//INT = ESP32_SPI_TFT, ESP32_I2C_TFT

template <uint8_t BPP, class INT>
class ESP32_Display: public NanoDisplayOps<BPP>,
                     public ESP32_TFT_Notifications
{
public:
    ESP32_Display( INT& tft ) 
        : NanoDisplayOps<BPP>(tft),
          m_tft(tft)
    {
        //tft.set_notify_base(this);
        this->m_w=tft.width();
        this->m_h=tft.height();
        //printf("PTR: %p (constructor)\n", &tft);
    }

    void begin()
    {
        //printf("PTR: %p (begin)\n", (ESP32_TFT_Notifications*)this);
        m_tft.set_notify_base(this);
        m_tft.begin(); 
    }    

    void end()
    { 
        m_tft.end(); 
    }


public:
    void on_rotation_changed() override
    {
            this->m_w=m_tft.width();
            this->m_h=m_tft.height();
    }

public:
    //direct TFT control, be carefull!!
    INT& tft() {return m_tft;}

private:
    INT& m_tft;
};

