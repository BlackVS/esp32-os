#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
//#include "driver/spi_master.h"
#include "esp32_tft_common.h"
#include "esp32_tft_constants.h"
#include "esp_log.h"

#include <cstring>

#include "esp32_i2c.h"

#define USE_POLLING_TRANSMIT 1


///////////////////////////////////////////////////////////////////////////////
#undef TAG
#define TAG "I2CBUS"

ESP32_I2Cdevice::ESP32_I2Cdevice(I2C_t& port, uint16_t dev_addr, gpio_num_t pin_reset)
:   m_port(port),
    m_pin_reset(pin_reset),
    m_dev_addr(dev_addr)
{

}

esp_err_t ESP32_I2Cdevice::begin() 
{
    //ESP_LOGD(TAG, "%s()", __FUNCTION__);
    return m_port.begin();
}

esp_err_t ESP32_I2Cdevice::reset(unsigned int delayMs)
{
    if ( m_pin_reset >= 0 )
    {
        gpio_set_direction( m_pin_reset, GPIO_MODE_OUTPUT );
        gpio_set_level(m_pin_reset, 1);
        tft_delay(10);
        gpio_set_level(m_pin_reset, 0);
        tft_delay(10);
        gpio_set_level(m_pin_reset, 1);
        tft_delay(delayMs);
    }
    return ESP_OK;
}

void ESP32_I2Cdevice::start_cmd()
{
    m_port.start(m_dev_addr);
    m_port.send(SSD1306_COMMAND);
}

void ESP32_I2Cdevice::start_data()
{
    m_port.start(m_dev_addr);
    m_port.send(SSD1306_DATA_CONTINUE);
}


void ESP32_I2Cdevice::write_byte (uint8_t  data)
{
    m_port.send(data);
}

void ESP32_I2Cdevice::write_bytes(const uint8_t* buf, unsigned int buflen)
{
    m_port.sendBuffer(buf, buflen);
}

void ESP32_I2Cdevice::stop()
{
    m_port.stop();
}

