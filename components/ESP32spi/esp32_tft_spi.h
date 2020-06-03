#pragma once

#include "driver/spi_master.h"
#include "driver/gpio.h"
//#include "log.h"
#include "v2/lcd/base/display_base.h"
#include "esp32_tft_common.h"
#include "esp32_spi.h"


class ESP32_SPI_TFT : public NanoDisplayInterface
{
        ESP32_SPIdevice&            m_device;
        ESP32_TFT_Notifications*    m_base={NULL};

        uint    m_width  = {0};
        uint    m_height = {0};
        uint    m_offset_x = {0};
        uint    m_offset_y = {0};
        TFT_ROTATION m_display_rotation={TFT_ROTATION_DEFAULT};
        TFT_TYPE m_display_type = {TFT_AUTO};


    public:
        ESP32_SPI_TFT(ESP32_SPIdevice& dev, 
                      uint width, uint height, uint offs_x=0, uint ofs_y=0, 
                      TFT_ROTATION display_rotation=TFT_ROTATION_DEFAULT, 
                      TFT_TYPE display_type=TFT_AUTO);

    public: //Display methods


        esp_err_t begin();
        void      end() {}


        //additional for NanoDisplay
        void send(uint8_t data) override
        {
            m_device.write_byte(data, true);
        }
        void startBlock(uint x, uint y, uint w)  override;
        void endBlock() override;
        void start() override {};
        void stop() override  {};
        void sendBuffer(const uint8_t *buffer, uint16_t size) override  {};
        void nextBlock() override  {};

        //notifications
        void set_notify_base(ESP32_TFT_Notifications* base)
        {
            m_base=base;
            //printf("PTR: %p (set_notify_base)\n", base);
        }

    protected:
        esp_err_t   init();
        void        configureDisplay(const uint8_t *config, uint8_t configSize);
        void        _set_rotation();

    public: //properties
        uint width()  { return m_width; }
        uint height() { return m_height; }
};



