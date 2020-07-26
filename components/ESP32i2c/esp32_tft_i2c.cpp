#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
//#include "driver/spi_master.h"
#include "esp_log.h"

#include "esp32_tools.h"
#include "esp32_tft_i2c.h"
#include "esp32_tft_constants.h"


#undef TAG
#define TAG "I2CTFT"

ESP32_I2C_TFT::ESP32_I2C_TFT(ESP32_I2Cdevice& device, unsigned int width, unsigned int height, TFT_TYPE display_type)
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
    SSD1306_DISPLAY_OFF,        0x00,       // display off
    SSD1306_MEMORY_ADDR_MODE,   0x01, 0x00, // Page horizontal Addressing mode
    SSD1306_COMSCANDEC,         0x00,       // Scan from 127 to 0 (Reverse scan)
    SSD1306_SETSTARTLINE| 0x00, 0x00,       // First line to start scanning from
    SSD1306_SETCONTRAST,        0x01, 0x7F, // contast value to 0x7F according to datasheet
    SSD1306_SEGREMAP    | 0x01, 0x00,       // Use reverse mapping. 0x00 - is normal mapping
    SSD1306_NORMAL_DISPLAY,     0x00,       // Normal display
    SSD1306_SETMULTIPLEX,       0x01, 63,   // Reset to default MUX. See datasheet
    SSD1306_SETDISPLAYOFFSET,   0x01, 0x00, // no offset
    SSD1306_SETDISPLAYCLOCKDIV, 0x01, 0x80, // set to default ratio/osc frequency
    SSD1306_SETPRECHARGE,       0x01, 0x22, // switch precharge to 0x22 // 0xF1
    SSD1306_SETCOMPINS,         0x01, 0x12, // set divide ratio
    SSD1306_SETVCOMDETECT,      0x01, 0x20, // vcom deselect to 0x20 // 0x40
    SSD1306_CHARGEPUMP,         0x01, 0x14, // Enable charge pump
    SSD1306_DISPLAYALLON_RESUME,0x00,       // Display resume
    SSD1306_DISPLAY_ON,         0x00,       // Display on
};


//#define SH1106_EXTERNALVCC
// static const PROGMEM uint8_t SH1106_128x64x1_INIT[] =
// {
// #ifdef SDL_EMULATION
//     SDL_LCD_SSD1306, 0x00,
//     0x00, 0x00,
// #endif
//     SSD1306_DISPLAY_OFF,        0x00,       // display off
//     SSD1306_SETDISPLAYCLOCKDIV, 0x01, 0x80, // set to default ratio/osc frequency
//     SSD1306_SETMULTIPLEX,       0x01, 0x3f,  // Reset to default MUX. See datasheet
//     SSD1306_SETDISPLAYOFFSET,   0x01, 0x00, // no offset
//     SSD1306_SETSTARTLINE| 0x00, 0x00,       // First line to start scanning from

// #ifdef SH1106_EXTERNALVCC
//     SSD1306_CHARGEPUMP,         0x01, 0x10, // Enable charge pump
// #else
//     SSD1306_CHARGEPUMP,         0x01, 0x14, // Enable charge pump
// #endif
//     SSD1306_MEMORY_ADDR_MODE,   0x01, 0x00, // Page horizontal Addressing mode
//     SSD1306_SEGREMAP    | 0x01, 0x00,       // Use reverse mapping. 0x00 - is normal mapping
//     SSD1306_COMSCANDEC,         0x00,       // Scan from 127 to 0 (Reverse scan)
//     SSD1306_SETCOMPINS,         0x01, 0x12, // set divide ratio

// #ifdef SH1106_EXTERNALVCC
//     SSD1306_SETCONTRAST,        0x01, 0x9F, // contast value to 0x7F according to datasheet
//     SSD1306_SETPRECHARGE,       0x01, 0x22, // switch precharge to 0x22 // 0xF1
// #else
//     SSD1306_SETCONTRAST,        0x01, 0xCF, // contast value to 0x7F according to datasheet
//     SSD1306_SETPRECHARGE,       0x01, 0xF1, // switch precharge to 0x22 // 0xF1
// #endif
//     SSD1306_SETVCOMDETECT,      0x01, 0x40, // vcom deselect to 0x20 // 0x40

//     SSD1306_NORMAL_DISPLAY,     0x00,       // Normal display
//     SSD1306_DISPLAYALLON_RESUME,0x00,       // Display resume
//     SSD1306_DISPLAY_ON,         0x00,       // Display on
// };


// static const PROGMEM uint8_t s_SH1106_lcd128x64_initData[] =
// {
// #ifdef SDL_EMULATION
//     SDL_LCD_SH1106, 0x00,
//     0x00, 0x00,
// #endif
//     0xAE, 0x00,        // display off
//     0xC8, 0x00,        // Scan from 127 to 0 (Reverse scan)
//     0x40| 0x00, 0x00,  // First line to start scanning from
//     0x81, 0x01, 0x7F,  // contast value to 0x7F according to datasheet
//     0xA0| 0x01, 0x00,  // Use reverse mapping. 0x00 - is normal mapping
//     0xA6, 0x00,        // Normal display
//     0xA8, 0x01, 63,    // Reset to default MUX. See datasheet
//     0xD3, 0x01, 0x00,  // no offset
//     0xD5, 0x01, 0x80,  // set to default ratio/osc frequency
//     0xD9, 0x01, 0x22,  // switch precharge to 0x22 // 0xF1
//     0xDA, 0x01, 0x12,  // set divide ratio com pins
//     0xDB, 0x01, 0x20,  // vcom deselect to 0x20 // 0x40
//     0x8D, 0x01, 0x14,  // Enable charge pump
//     0xA4, 0x00,        // Display on resume
//     0xA5, 0x00,        // Display on
// };


static const PROGMEM uint8_t SH1106_128x64x1_VCCINT_INIT[] =
{
    SSD1306_DISPLAY_OFF,        0x00,       // display off
    SSD1306_SETDISPLAYCLOCKDIV, 0x01, 0x80, // set to default ratio/osc frequency
    SSD1306_SETMULTIPLEX,       0x01, 0x3f,  // Reset to default MUX. See datasheet
    SSD1306_SETDISPLAYOFFSET,   0x01, 0x00, // no offset
    SSD1306_SETSTARTLINE| 0x00, 0x00,       // First line to start scanning from
    SSD1306_CHARGEPUMP,         0x01, 0x14, // Enable charge pump
    SSD1306_SEGREMAP    | 0x01, 0x00,       // Use reverse mapping. 0x00 - is normal mapping
    SSD1306_COMSCANDEC,         0x00,       // Scan from 127 to 0 (Reverse scan)
    SSD1306_SETCOMPINS,         0x01, 0x12, // set divide ratio
    SSD1306_SETCONTRAST,        0x01, 0x7F, // contast value to 0x7F according to datasheet cf 7f?
    SSD1306_SETPRECHARGE,       0x01, 0x22, // switch precharge to 0x22 0xF1 0x1f ?
    SSD1306_SETVCOMDETECT,      0x01, 0x20, // vcom deselect to 0x20 // 0x40

    //SSD1306_MEMORY_ADDR_MODE,   0x01, 0x00, // Page horizontal Addressing mode
    SSD1306_NORMAL_DISPLAY,     0x00,       // Normal display
    SSD1306_DISPLAYALLON_RESUME,0x00,       // Display resume
    SSD1306_DISPLAY_ON,         0x00,       // Display on
};


esp_err_t ESP32_I2C_TFT::init()
{
    m_device.reset();
    // Give LCD some time to initialize. Refer to SSD1306 datasheet
    delay(0);

    switch(m_display_type)
    {
        case TFT_SSD1306_128x64x1:
            //SPI_LOGD(TAGSPI,"ST7735_128x128x16");
            configureDisplay(SSD1306_128x64x1_INIT, sizeof(SSD1306_128x64x1_INIT));
            break;
        case TFT_SH1106_128x64x1:
            configureDisplay(SH1106_128x64x1_VCCINT_INIT, sizeof(SH1106_128x64x1_VCCINT_INIT));
            break;
        default:
            ESP_LOGE(TAG, "Unsupported display type, type=%d", (int)m_display_type);
            return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

void ESP32_I2C_TFT::configureDisplay(const uint8_t *config, uint8_t configSize)
{
    m_device.start_cmd();
    unsigned int i=0;
    while(i<configSize){
        uint8_t cmd=config[i++];
        uint8_t cnt=config[i++];
        m_device.write_byte(cmd);
        if(cnt) {
            m_device.write_bytes(&config[i], cnt);
            i+=cnt;
        }
    }
    m_device.stop();
}

void ESP32_I2C_TFT::startBlock(unsigned int x, unsigned int y, unsigned int w)
{
    m_block_y = y;
    m_block_x = x;
    switch(m_display_type)
    {
        case TFT_SSD1306_128x64x1:
            m_device.start_cmd();
            m_device.write_byte(SSD1306_SET_COLUMN_ADDR); 
            m_device.write_byte(x);
            m_device.write_byte(w ? (x + w - 1) : (m_width - 1));
            m_device.write_byte(SSD1306_SET_PAGE_ADDR);
            m_device.write_byte(y);
            m_device.write_byte((m_height >> 3) - 1);
            m_device.stop();
            m_device.start_data();
            break;
        case TFT_SH1106_128x64x1:
            m_device.start_cmd();
            m_device.write_byte(SH1106_SETPAGEADDRESS | y); 
            m_device.write_byte(SSD1306_SETHIGHCOLUMN | ((x+2)>>4)); 
            m_device.write_byte(SSD1306_SETLOWCOLUMN | ((x+2) & 0x0f)); 
            m_device.stop();
            m_device.start_data();
            break;
        default:
            ESP_LOGE(TAG, "Unsupported display type, type=%d", (int)m_display_type);
    }
}

void ESP32_I2C_TFT::endBlock()
{
    m_device.stop();
};

void ESP32_I2C_TFT::nextBlock()
{
    switch(m_display_type)
    {
        case TFT_SSD1306_128x64x1:
            break;
        case TFT_SH1106_128x64x1:
            m_device.stop();
            startBlock(m_block_x, m_block_y+1, 0);
            break;
        default:
            ESP_LOGE(TAG, "Unsupported display type, type=%d", (int)m_display_type);
    }
  
};

void ESP32_I2C_TFT::send(uint8_t data)
{
    m_device.write_byte(data);
};

void ESP32_I2C_TFT::sendBuffer(const uint8_t *buffer, uint16_t size)
{
    m_device.write_bytes(buffer, size);
};


void ESP32_I2C_TFT::start()
{
    assert(0);
};

void ESP32_I2C_TFT::stop()
{
    assert(0);
};


//notifications
void ESP32_I2C_TFT::set_notify_base(ESP32_TFT_Notifications* base)
{
    m_base=base;
    //printf("PTR: %p (set_notify_base)\n", base);
}


void ESP32_I2C_TFT::display_off()
{
    m_device.start_cmd();
    m_device.write_byte(SSD1306_DISPLAY_OFF); 
    m_device.stop();
}

void ESP32_I2C_TFT::display_on()
{
    m_device.start_cmd();
    m_device.write_byte(SSD1306_DISPLAY_ON);
    m_device.stop();
}

void ESP32_I2C_TFT::display_mode_normal()
{
    m_device.start_cmd();
    m_device.write_byte(SSD1306_NORMAL_DISPLAY); // Normal display
    m_device.stop();
}

void ESP32_I2C_TFT::display_mode_inverse()
{
    m_device.start_cmd();
    m_device.write_byte(SSD1306_INVERT_DISPLAY); // Invert display
    m_device.stop();
}


void ESP32_I2C_TFT::display_flip_horizontal(bool mode)
{
    m_device.start_cmd();
    m_device.write_byte(0xA0 | (mode ? 0x00: 0x01 ) ); // Invert display
    m_device.stop();
}

void ESP32_I2C_TFT::display_flip_vertical(bool mode)
{
    m_device.start_cmd();
    m_device.write_byte(mode ? 0xC0 : 0xC8); // Invert display
    m_device.stop();
}




