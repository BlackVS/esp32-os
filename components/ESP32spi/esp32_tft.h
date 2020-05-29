#pragma once

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "log.h"

#include "esp32_spi.h"

typedef enum {
    TFT_DEFAULT=-1,
    TFT_ST7735_128x128x16,
    TFT_ST7735_128x160x16,
    TFT_ST7735_M5STICKC,
} TFT_TYPE;

class ESP32_SPI_TFT
{
        ESP32_SPIdevice&   m_device;
        uint    m_width  = {0};
        uint    m_height = {0};
        uint    m_offset_x = {0};
        uint    m_offset_y = {0};
        TFT_TYPE m_display_type = {TFT_DEFAULT};


    public:
        ESP32_SPI_TFT(ESP32_SPIdevice& dev, uint width, uint height, uint offs_x=0, uint ofs_y=0, TFT_TYPE display_type=TFT_DEFAULT);

    public: //Display methods


        esp_err_t begin();
        void      end() {}


        // void stop()
        // {
        //     ESP_LOGD("ESP32_SPI_TFT"," %s()", __FUNCTION__);
        // }

        //additional for NanoDisplay

        void send(uint8_t data)
        {
            m_device.write_byte(data, true);
        }

        void startBlock(uint x, uint y, uint w);
        void endBlock();

    protected:
        esp_err_t   init();
        void        configureDisplay(const uint8_t *config, uint8_t configSize);

    public: //properties
        uint width()  { return m_width; }
        uint height() { return m_height; }
};



