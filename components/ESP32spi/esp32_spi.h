#pragma once

#include "driver/spi_master.h"
#include "driver/gpio.h"
//#include "log.h"


#define SPIHOST2STR(h) (((h)==VSPI_HOST)?"VSPI_HOST":(((h)==HSPI_HOST)?"HSPI_HOST":"SPI_HOST" ))

//Full screemn - fastest
#define SPI_DATA_BUF_SIZE 80*160*2

////////////////////////////////////////////////////////////
class ESP32_SPIbus
{
    spi_host_device_t m_host;
    gpio_num_t m_pin_mosi = {GPIO_NUM_NC};
    gpio_num_t m_pin_miso = {GPIO_NUM_NC};
    gpio_num_t m_pin_sclk = {GPIO_NUM_NC};
    int        m_max_transfer_sz = {SPI_MAX_DMA_LEN};
    
    //state
    bool  m_inited={false};

public:
    ESP32_SPIbus( spi_host_device_t host, gpio_num_t  pin_mosi, gpio_num_t pin_miso, gpio_num_t pin_sclk, int max_transfer_sz=SPI_DATA_BUF_SIZE);
    esp_err_t begin();
    esp_err_t end();

public://properties
    spi_host_device_t get_host_id()  { return m_host; }
    const char*       get_host_str() { return SPIHOST2STR(m_host); }
};



class ESP32_SPIdevice
{
        ESP32_SPIbus&   m_bus;
        gpio_num_t      m_pin_reset;
        gpio_num_t      m_pin_cs;
        gpio_num_t      m_pin_dc;
        int             m_freq;
        //
        bool                m_inited={false};
        spi_device_handle_t m_dev_handle={NULL};
        
        //
        uint8_t  m_data_buf[SPI_DATA_BUF_SIZE];
        uint     m_data_size = {0};

    public:
        ESP32_SPIdevice(ESP32_SPIbus& bus, gpio_num_t pin_reset, gpio_num_t pin_cs, gpio_num_t pin_dc, int freq=27000000)
        :   m_bus(bus),
            m_pin_reset(pin_reset),
            m_pin_cs(pin_cs),
            m_pin_dc(pin_dc),
            m_freq(freq)
        {

        }

        ~ESP32_SPIdevice() {}

        esp_err_t begin();
        esp_err_t end();

        esp_err_t reset(uint delayMs = 20);
        
        esp_err_t write_byte (uint8_t  data, bool bCaching=false);
        esp_err_t flush_data();

        esp_err_t write_bytes(const uint8_t* buf, uint buflen);
        esp_err_t write_cmd(uint8_t cmd);
        esp_err_t write_16bits(uint16_t word);
        esp_err_t write_2x16bits(uint16_t word1, uint16_t word2);

        esp_err_t set_data_mode();
        esp_err_t set_cmd_mode();

};


void spi_delay(unsigned long ms);

