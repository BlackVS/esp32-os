#pragma once

//#include "driver/spi_master.h"
#include "driver/gpio.h"
//#include "log.h"
#include "v2/lcd/display_base.h"
#include "esp32_tft_common.h"
#include "I2Cbus.hpp"
#include "esp32_i2c.h"

class ESP32_I2C_TFT : public NanoDisplayInterface
{
        ESP32_I2Cdevice&  m_device;
        unsigned int    m_width  = {0};
        unsigned int    m_height = {0};
        TFT_TYPE m_display_type={TFT_UNKNOWN};
        ESP32_TFT_Notifications*    m_base={NULL};

    public:
        ESP32_I2C_TFT(ESP32_I2Cdevice& device, unsigned int width, unsigned int height, TFT_TYPE display_type=TFT_AUTO);

        esp_err_t begin();
        void      end() {}

  

    //NanoDisplayInterface
    public:
        void start() override;
        void stop()   override;
        void send(uint8_t data)  override;
        void sendBuffer(const uint8_t *buffer, uint16_t size)  override;
        void startBlock(unsigned int x, unsigned int y, unsigned int w) override;
        void nextBlock()  override;
        void endBlock()  override;

    //ESP32_TFT_Notifications
    public:
        void set_notify_base(ESP32_TFT_Notifications* base);

    //extra methods for display
    public:
        void display_off();
        void display_on();
        void display_mode_normal();
        void display_mode_inverse();
        void display_flip_horizontal(bool mode);
        void display_flip_vertical(bool mode);

    protected:
        esp_err_t   init();
        void        configureDisplay(const uint8_t *config, uint8_t configSize);

    public: //properties
        unsigned int width()  { return m_width; }
        unsigned int height() { return m_height; }        
};



