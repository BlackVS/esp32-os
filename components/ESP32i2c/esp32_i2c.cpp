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

esp_err_t ESP32_I2Cdevice::reset(uint delayMs)
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

void ESP32_I2Cdevice::write_bytes(const uint8_t* buf, uint buflen)
{
    m_port.sendBuffer(buf, buflen);
}

void ESP32_I2Cdevice::stop()
{
    m_port.stop();
}


// template <class I>
// void InterfaceSSD1306<I>::setStartLine(uint8_t line)
// {
//     m_startLine = line;
//     commandStart();
//     this->send( 0x40 | (line & 0x3F) ); // start line
//     this->stop();
// }

// template <class I>
// uint8_t InterfaceSSD1306<I>::getStartLine()
// {
//     return m_startLine;
// }

// template <class I>
// void InterfaceSSD1306<I>::normalMode()
// {
//     commandStart();
//     this->send(0xA6); // Normal display
//     this->stop();
// }

// template <class I>
// void InterfaceSSD1306<I>::invertMode()
// {
//     commandStart();
//     this->send(0xA7); // Invert display
//     this->stop();
// }

// template <class I>
// void InterfaceSSD1306<I>::setContrast(uint8_t contrast)
// {
//     commandStart();
//     this->send(0x81); // set contrast
//     this->send(contrast);
//     this->stop();
// }

// template <class I>
// void InterfaceSSD1306<I>::displayOff()
// {
//     commandStart();
//     this->send(0xAE); // display off
//     this->stop();
// }

// template <class I>
// void InterfaceSSD1306<I>::displayOn()
// {
//     commandStart();
//     this->send(0xAF); // display on
//     this->stop();
// }

// template <class I>
// void InterfaceSSD1306<I>::flipHorizontal(uint8_t mode)
// {
//     commandStart();
//     this->send( 0xA0 | (mode ? 0x00: 0x01 ) ); // seg remap
//     this->stop();
// }

// template <class I>
// void InterfaceSSD1306<I>::flipVertical(uint8_t mode)
// {
//     commandStart();
//     this->send( mode ? 0xC0 : 0xC8 );
//     this->stop();
// }
