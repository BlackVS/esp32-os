#pragma once

//#include "driver/spi_master.h"
#include "driver/gpio.h"
//#include "log.h"
#include "v2/lcd/base/display_base.h"
#include "esp32_tft_common.h"
#include "I2Cbus.hpp"
#include "esp32_i2c.h"

class ESP32_I2C_TFT : public NanoDisplayInterface
{
        ESP32_I2Cdevice&  m_device;
        uint    m_width  = {0};
        uint    m_height = {0};
        TFT_TYPE m_display_type={TFT_UNKNOWN};
        ESP32_TFT_Notifications*    m_base={NULL};

    public:
        ESP32_I2C_TFT(ESP32_I2Cdevice& device, uint width, uint height, TFT_TYPE display_type=TFT_AUTO);

        esp_err_t begin();
        void      end() {}

  

    //NanoDisplayInterface
    public:
        void start() override 
        {
            assert(0);
        };
        void stop()   override 
        {
            assert(0);
        };
        void send(uint8_t data)  override 
        {
            m_device.write_byte(data);
        };
        void sendBuffer(const uint8_t *buffer, uint16_t size)  override 
        {
            m_device.write_bytes(buffer, size);
        };
        void startBlock(uint x, uint y, uint w) override;
        void nextBlock()  override {};
        void endBlock()  override 
        {
            m_device.stop();
        };

        //notifications
        void set_notify_base(ESP32_TFT_Notifications* base)
        {
            m_base=base;
            //printf("PTR: %p (set_notify_base)\n", base);
        }

    protected:
        esp_err_t   init();
        void        configureDisplay(const uint8_t *config, uint8_t configSize);
        void        _set_rotation() 
        {
            assert(0);
        }

    public: //properties
        uint width()  { return m_width; }
        uint height() { return m_height; }        
};



