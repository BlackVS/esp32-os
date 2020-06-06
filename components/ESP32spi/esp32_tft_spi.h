#pragma once

#include "driver/spi_master.h"
#include "driver/gpio.h"
//#include "log.h"
#include "v2/lcd/display_base.h"
#include "esp32_tft_common.h"
#include "esp32_spi.h"


class ESP32_SPI_TFT : public NanoDisplayInterface
{
        ESP32_SPIdevice&            m_device;
        ESP32_TFT_Notifications*    m_base={NULL};

        unsigned int    m_width  = {0};
        unsigned int    m_height = {0};
        unsigned int    m_offset_x = {0};
        unsigned int    m_offset_y = {0};
        TFT_ROTATION m_display_rotation={TFT_ROTATION_DEFAULT};
        TFT_TYPE m_display_type = {TFT_AUTO};


    public:
        ESP32_SPI_TFT(ESP32_SPIdevice& dev, 
                      unsigned int width, unsigned int height, unsigned int offs_x=0, unsigned int ofs_y=0, 
                      TFT_ROTATION display_rotation=TFT_ROTATION_DEFAULT, 
                      TFT_TYPE display_type=TFT_AUTO);

    public: //Display methods


        esp_err_t begin();
        void      end() {}


    //NanoDisplayInterface
    public:
        
        void send(uint8_t data) override;
        void startBlock(unsigned int x, unsigned int y, unsigned int w)  override;
        void endBlock() override;
        void start() override {};
        void stop() override  {};
        void sendBuffer(const uint8_t *buffer, uint16_t size) override  {};
        void nextBlock() override  {};

    //ESP32_TFT_Notifications
    public:
        void set_notify_base(ESP32_TFT_Notifications* base);

    protected:
        esp_err_t   init();
        void        configureDisplay(const uint8_t *config, uint8_t configSize);
        void        _set_rotation();

    public: //properties
        unsigned int width()  { return m_width; }
        unsigned int height() { return m_height; }
};



