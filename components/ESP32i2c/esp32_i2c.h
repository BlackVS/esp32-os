#pragma once

#include "I2Cbus.hpp"
#include "driver/gpio.h"
//#include "log.h"


////////////////////////////////////////////////////////////
class ESP32_I2Cdevice
{
        I2C_t&      m_port;
        gpio_num_t  m_pin_reset = {GPIO_NUM_NC};
        uint16_t    m_dev_addr = {0};
        //

    public:
        ESP32_I2Cdevice(I2C_t& port, uint16_t dev_addr, gpio_num_t pin_reset=GPIO_NUM_NC);
        ~ESP32_I2Cdevice() {}

        esp_err_t begin();
        //esp_err_t end();

        esp_err_t reset(uint delayMs = 20);
        
        // i2c communication
        void start_cmd();
        void start_data();

        void write_byte (uint8_t  data);  
        void write_bytes(const uint8_t* buf, uint buflen);
        
        //void start();
        void stop();

        // esp_err_t flush_data();

        // esp_err_t write_cmd(uint8_t cmd);
        // esp_err_t write_16bits(uint16_t word);
        // esp_err_t write_2x16bits(uint16_t word1, uint16_t word2);

        // esp_err_t set_data_mode();
        // esp_err_t set_cmd_mode();

};


