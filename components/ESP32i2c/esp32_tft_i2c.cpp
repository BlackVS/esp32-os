#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
//#include "driver/spi_master.h"
#include "esp_log.h"

#include "esp32_tft_i2c.h"
#include "esp32_tft_constants.h"


#undef TAG
#define TAG "I2CTFT"

ESP32_I2C_TFT::ESP32_I2C_TFT(ESP32_I2Cdevice& device, uint width, uint height, TFT_TYPE display_type)
: m_device(device),
  m_width(width),
  m_height(height),
  m_display_type(display_type)
{

}

esp_err_t ESP32_I2C_TFT::begin()
{
    //ESP_LOGD(TAG, "%s()", __FUNCTION__);
	m_device.begin();
    init();//send initial data to TFT
    return ESP_OK;
}


static const PROGMEM uint8_t SSD1306_128x64x1_INIT[] =
{
#ifdef SDL_EMULATION
    SDL_LCD_SSD1306, 0x00,
    0x00, 0x00,
#endif
    0xAE, 0x00,          // display off
    0x20, 0x01, 0x00,    // Page horizontal Addressing mode
    0xC8, 0x00,          // Scan from 127 to 0 (Reverse scan)
    0x40| 0x00, 0x00,    // First line to start scanning from
    0x81, 0x01, 0x7F,    // contast value to 0x7F according to datasheet
    0xA0| 0x01, 0x00,    // Use reverse mapping. 0x00 - is normal mapping
    0xA6, 0x00,          // Normal display
    0xA8, 0x01, 63,      // Reset to default MUX. See datasheet
    0xD3, 0x01, 0x00,    // no offset
    0xD5, 0x01, 0x80,    // set to default ratio/osc frequency
    0xD9, 0x01, 0x22,    // switch precharge to 0x22 // 0xF1
    0xDA, 0x01, 0x12,    // set divide ratio
    0xDB, 0x01, 0x20,    // vcom deselect to 0x20 // 0x40
    0x8D, 0x01, 0x14,    // Enable charge pump
    0xA4, 0x00,          // Display resume
    0xAF, 0x00,          // Display on
};


esp_err_t ESP32_I2C_TFT::init()
{
    m_device.reset();
    // Give LCD some time to initialize. Refer to SSD1306 datasheet
    tft_delay(0);

    switch(m_display_type)
    {
        case TFT_SSD1306_128x64x1:
            //SPI_LOGD(TAGSPI,"ST7735_128x128x16");
            configureDisplay(SSD1306_128x64x1_INIT, sizeof(SSD1306_128x64x1_INIT));
            break;
        default:
            ESP_LOGE(TAG, "Unsupported display type, type=%d", (int)m_display_type);
            return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

#define CMD_DELAY 0xFF
void ESP32_I2C_TFT::configureDisplay(const uint8_t *config, uint8_t configSize)
{
    uint8_t command = 1;
    int8_t args;
    m_device.start_cmd();
    for( uint8_t i=0; i<configSize; i++)
    {
        uint8_t data = pgm_read_byte(&config[i]);
        if ( command )
        {
            if ( command == CMD_DELAY )
            {
                command = 1;
                tft_delay( data == 0xFF ? 500: data );
            }
            else
            {
                m_device.write_byte(data);
                command = 0;
                args = -1;
            }
        }
        else
        {
            if (args < 0)
            {
                if ( data >= 128 )
                {
                    command = data;
                }
                else if ( data > 0 )
                {
                    args = data;
                }
                else
                {
                    command = 1;
                }
            }
            else
            {
                args--;
                m_device.write_byte(data);
                if ( !args )
                {
                    command = 1;
                }
            }
        }
    }
    m_device.stop();
}

void ESP32_I2C_TFT::startBlock(lcduint_t x, lcduint_t y, lcduint_t w)
{
    m_device.start_cmd();
    m_device.write_byte(SSD1306_SET_COLUMN_ADDR); 
    m_device.write_byte(x);
    m_device.write_byte(w ? (x + w - 1) : (m_width - 1));
    m_device.write_byte(SSD1306_SET_PAGE_ADDR);
    m_device.write_byte(y);
    m_device.write_byte((m_height >> 3) - 1);
    //send data 
    m_device.stop();
    m_device.start_data();
}

// template <class I>
// void InterfaceSSD1306<I>::nextBlock()
// {

// }

// template <class I>
// void InterfaceSSD1306<I>::endBlock()
// {
//     this->stop();
// }