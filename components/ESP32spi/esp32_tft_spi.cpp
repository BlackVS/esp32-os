#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"

#include "esp32_tft_spi.h"
#include "esp32_tft_constants.h"

///////////////////////////////////////////////////////////////////////////////

#define CMDDELAY 0xFF


#define SWAP(a, b) do { typeof(a) temp = a; a = b; b = temp; } while (0)

static const uint8_t ST7735_128x128x16_INIT[] =
{
    0x01, CMDDELAY,  150,   // SWRESET sw reset. not needed, we do hardware reset
    0x11, CMDDELAY,  255,   // SLPOUT exit sleep mode
    0xB1, 0x03, 0x01, 0x2C, 0x2D,  // FRMCTR1 frame rate control 1, use by default
    0xB2, 0x03, 0x01, 0x2C, 0x2D,  // FRMCTR2, Frame Rate Control (In Idle mode/ 8-colors)
    0xB3, 0x06,              // FRMCTR3 (B3h): Frame Rate Control (In Partial mode/ full colors)
                0x01, 0x2C, 0x2D,
                0x01, 0x2C, 0x2D,
    0xB4, 0x01, 0x07,        // INVCTR display inversion, use by default
    0xB6, 0x02, 0x15, 0x02,  // DISSET5
    0xC0, 0x03, 0xA2, 0x02, 0x84,  // PWCTR1 power control 1
    0xC1, 0x01, 0xC5,        // PWCTR2 power control 2
    0xC2, 0x02, 0x0A, 0x00,  // PWCTR3 power control 3
    0xC3, 0x02, 0x8A, 0x2A,  // PWCTR4 (C3h): Power Control 4 (in Idle mode/ 8-colors)
    0xC4, 0x02, 0x8A, 0xEE,  // PWCTR5 (C4h): Power Control 5 (in Partial mode/ full-colors)
    0xC5, 0x01, 0x0E,        // VMCTR vcom control 1
    0x20, 0x00,              // INVOFF (20h): Display Inversion Off
//    0xFC, 0x02, 0x11, 0x15,  // PWCTR6
    0x36, 0x01, 0b00000000,  // MADCTL
    0x3A, 0x01, 0x05,        // COLMOD set 16-bit pixel format
//    0x26, 1, 0x08,        // GAMSET set gamma curve: valid values 1, 2, 4, 8
//    0xF2, 1, 0x01,        // enable gamma adjustment, 0 - to disable
    0xE0, 0x10, // GMCTRP1 positive gamma correction
                0x0F, 0x1A, 0x0F, 0x18,
                0x2F, 0x28, 0x20, 0x22,
                0x1F, 0x1B, 0x23, 0x37,
                0x00, 0x07, 0x02, 0x10,
    0xE1, 0x10, // GMCTRN1 negative gamma correction
                0x0F, 0x1B, 0x0F, 0x17,
                0x33, 0x2C, 0x29, 0x2E,
                0x30, 0x30, 0x39, 0x3F,
                0x00, 0x07, 0x03, 0x10,
//    0xC7,  1,  0x40,                // vcom offset
//    0x2A,  CMD_ARG,  0x00, CMD_ARG, 0x00, CMD_ARG, 0x00, CMD_ARG, 0x7F,   // set column address, not needed. set by direct API
//    0x2B,  CMD_ARG,  0x00, CMD_ARG, 0x00, CMD_ARG, 0x00, CMD_ARG, 0x9F,   // set page address, not needed. set by direct API
    0x29, CMDDELAY,  100, // DISPON display on
    0x13, CMDDELAY,   10, // NORON
};

static const uint8_t ST7735_128x160x16_INIT[] =
{
    0x01, CMDDELAY,  150,   // SWRESET sw reset. not needed, we do hardware reset
    0x11, CMDDELAY,  255,   // SLPOUT exit sleep mode
    0xB1, 0x03, 0x01, 0x2C, 0x2D,  // FRMCTR1 frame rate control 1, use by default
    0xB2, 0x03, 0x01, 0x2C, 0x2D,  // FRMCTR2, Frame Rate Control (In Idle mode/ 8-colors)
    0xB3, 0x06,              // FRMCTR3 (B3h): Frame Rate Control (In Partial mode/ full colors)
                0x01, 0x2C, 0x2D,
                0x01, 0x2C, 0x2D,
    0xB4, 0x01, 0x07,        // INVCTR display inversion, use by default
    0xB6, 0x02, 0x15, 0x02,  // DISSET5
    0xC0, 0x03, 0xA2, 0x02, 0x84,  // PWCTR1 power control 1
    0xC1, 0x01, 0xC5,        // PWCTR2 power control 2
    0xC2, 0x02, 0x0A, 0x00,  // PWCTR3 power control 3
    0xC3, 0x02, 0x8A, 0x2A,  // PWCTR4 (C3h): Power Control 4 (in Idle mode/ 8-colors)
    0xC4, 0x02, 0x8A, 0xEE,  // PWCTR5 (C4h): Power Control 5 (in Partial mode/ full-colors)
    0xC5, 0x01, 0x0E,        // VMCTR vcom control 1
    0x20, 0x00,              // INVOFF (20h): Display Inversion Off
//    0xFC, 0x02, 0x11, 0x15,  // PWCTR6
    0x36, 0x01, 0b00000000,  // MADCTL
    0x3A, 0x01, 0x05,        // COLMOD set 16-bit pixel format
//    0x26, 1, 0x08,        // GAMSET set gamma curve: valid values 1, 2, 4, 8
//    0xF2, 1, 0x01,        // enable gamma adjustment, 0 - to disable
    0xE0, 0x10, // GMCTRP1 positive gamma correction
                0x0F, 0x1A, 0x0F, 0x18,
                0x2F, 0x28, 0x20, 0x22,
                0x1F, 0x1B, 0x23, 0x37,
                0x00, 0x07, 0x02, 0x10,
    0xE1, 0x10, // GMCTRN1 negative gamma correction
                0x0F, 0x1B, 0x0F, 0x17,
                0x33, 0x2C, 0x29, 0x2E,
                0x30, 0x30, 0x39, 0x3F,
                0x00, 0x07, 0x03, 0x10,
//    0xC7,  1,  0x40,                // vcom offset
//    0x2A,  CMD_ARG,  0x00, CMD_ARG, 0x00, CMD_ARG, 0x00, CMD_ARG, 0x7F,   // set column address, not needed. set by direct API
//    0x2B,  CMD_ARG,  0x00, CMD_ARG, 0x00, CMD_ARG, 0x00, CMD_ARG, 0x9F,   // set page address, not needed. set by direct API
    0x29, CMDDELAY,  100, // DISPON display on
    0x13, CMDDELAY,   10, // NORON
};

static const uint8_t ST7735_80x160x16_INIT[] =
{
    0x01, CMDDELAY,  150,         // SWRESET sw reset. not needed, we do hardware reset
    0x11, CMDDELAY,  255,         // SLPOUT exit sleep mode
    0xB1, 0x03, 0x01, 0x2C, 0x2D,  // Rate = fosc/(1x2+40) * (LINE+2C+2D)
    0xB2, 0x03, 0x01, 0x2C, 0x2D,  // FRMCTR2, Rate = fosc/(1x2+40) * (LINE+2C+2D)
    0xB3, 0x06,                    // FRMCTR3 (B3h): Frame Rate Control (In Partial mode/ full colors)
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,  
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      0xC8,                   //     row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05,                   //     16-bit color
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 0
      0x00, 0x7F+0x02,        //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x01,             //     XSTART = 0
      0x00, 0x9F+0x01,        //     XEND = 159
//++
    ST7735_INVON , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_GMCTRP1, 16      , //  1: 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    0x13, CMDDELAY,   10, // NORON
    0x29, CMDDELAY,  100, // DISPON display on
};


/////////////////////////////////////////////////////////////////////////////////

#undef TAG
#define TAG "SPITFT"

ESP32_SPI_TFT::ESP32_SPI_TFT(ESP32_SPIdevice& dev, unsigned int width, unsigned int height, unsigned int ofs_x, unsigned int ofs_y, 
      TFT_ROTATION display_rotation, 
      TFT_TYPE display_type)
 : m_device(dev),
   m_width(width),
   m_height(height),
   m_offset_x(ofs_x),
   m_offset_y(ofs_y),
   m_display_rotation(display_rotation),
   m_display_type(display_type)
{

}

esp_err_t ESP32_SPI_TFT::begin()
{
    //ESP_LOGD(TAG, "%s()", __FUNCTION__);
	  m_device.begin();
    init();//send initial data to TFT
    return ESP_OK;
}

#define CMD_DELAY 0xff
void ESP32_SPI_TFT::configureDisplay(const uint8_t *config, uint8_t configSize)
{
    int i=0;
    while(i<configSize)
    {
        uint8_t cmd = config[i++];
        int arg_len = config[i++];
        //single cmd + delay
        if( arg_len==CMDDELAY) {
            m_device.write_cmd(cmd);
            spi_delay(config[i++]);
            continue;
        }
        //complex command
        m_device.write_cmd(cmd);
        if(arg_len) {
            m_device.set_data_mode();
            m_device.write_bytes(&config[i], arg_len);
            i+=arg_len;
        }

    }
}

esp_err_t ESP32_SPI_TFT::init()
{
    //ESP_LOGD(TAG, "%s()", __FUNCTION__);
    // Give LCD some time to initialize. Refer to ST7735 datasheet
    spi_delay(120);
    
    TFT_TYPE dtype=m_display_type;
    if(dtype==TFT_AUTO) 
    {
        if(m_width==128 && m_height==128)
            dtype=TFT_ST7735_128x128x16;
        else 
        if(m_width==128 && m_height==160)
            dtype=TFT_ST7735_128x160x16;
        else
        if(m_width==80 && m_height==160)
            dtype=TFT_ST7735_M5STICKC;
        else
            dtype=TFT_ST7735_128x128x16;

    }
    ESP_LOGD(TAG, "DTYPE=%d", (int)dtype);
    switch(dtype)
    {
        case TFT_ST7735_128x128x16:
            //SPI_LOGD(TAGSPI,"ST7735_128x128x16");
            configureDisplay(ST7735_128x128x16_INIT, sizeof(ST7735_128x128x16_INIT));
            break;
        case TFT_ST7735_128x160x16:
            //SPI_LOGD(TAGSPI,"ST7735_128x160x16");
            configureDisplay(ST7735_128x160x16_INIT, sizeof(ST7735_128x160x16_INIT));
            break;
        case TFT_ST7735_M5STICKC:
            {
                //SPI_LOGD(TAGSPI,"ST7735_M5STICKC");
                configureDisplay(ST7735_80x160x16_INIT, sizeof(ST7735_80x160x16_INIT));
            }
            break;
        default:
            //SPI_LOGD(TAGSPI,"ST7735_UNKNOWN");
            break;
    }
    _set_rotation();
    return ESP_OK;
}


void ESP32_SPI_TFT::send(uint8_t data)
{
    m_device.write_byte(data, true);
}


void ESP32_SPI_TFT::startBlock(unsigned int x1, unsigned int y, unsigned int w)
{
    //SPI_LOGD(TAGSPI,"%s", __FUNCTION__);
    
    unsigned int x2 = w ? (x1 + w - 1) : ( m_width - 1);
    x2 = (x2 < m_width) ? x2 : (m_width-1);

    //X
    m_device.write_cmd(0x2A);
    m_device.set_data_mode(); // According to datasheet all args must be passed in data mode
    m_device.write_2x16bits( x1 + m_offset_x, x2 + m_offset_x );
    //m_device.write_16bits( x1 + m_offset_x );
    //m_device.write_16bits( x2 + m_offset_x );
    //Y
    m_device.write_cmd(0x2B);
    m_device.set_data_mode(); // According to datasheet all args must be passed in data mode
    //m_device.write_16bits( y + m_offset_y );
    //m_device.write_16bits( m_height - 1 + m_offset_y );
    m_device.write_2x16bits( y + m_offset_y, m_height - 1 + m_offset_y );
    //Write
    m_device.write_cmd(0x2C);
    m_device.set_data_mode(); // According to datasheet all args must be passed in data mode
    // if m_dc=-1 ->  this->send(0x40);
}

void ESP32_SPI_TFT::endBlock()
{
    //in the case of buffered write - flush buffer
    m_device.flush_data();
}


const uint8_t _ST7735_M5STICKC[TFT_ROTATION_MAX] = {
  TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_MH | TFT_MAD_BGR,
  TFT_MAD_MV | TFT_MAD_MY | TFT_MAD_BGR,
  TFT_MAD_BGR,
  TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_BGR,
};

// ram_mode = 0b00000000;
// ram_mode = 0b01100000;
// ram_mode = 0b11000000;
// ram_mode = 0b10100000;

void ESP32_SPI_TFT::_set_rotation()
{
    //if ((rotation^m_display_rotation) & 0x01) - in the case of outer call
    if (m_display_rotation & 0x01) //first time
    {
        unsigned int t;
        //SWAP(m_offset_x, m_offset_y);
        t=m_offset_x; m_offset_x=m_offset_y; m_offset_y=t;
        //SWAP(m_width, m_height);
        t=m_width; m_width=m_height; m_height=t;
    }

    m_device.write_cmd(ST7735_DISPOFF);
    m_device.write_byte(TFT_MADCTL);
    m_device.set_data_mode();

    uint8_t code=TFT_MAD_BGR;
    switch(m_display_type)
    {
      case TFT_ST7735_M5STICKC:
        code=_ST7735_M5STICKC[m_display_rotation];
      default:
        break;
    }
    m_device.write_byte(code);
    m_device.write_cmd(ST7735_DISPON);

    if(m_base)
      m_base->on_rotation_changed();
}


  void ESP32_SPI_TFT::set_notify_base(ESP32_TFT_Notifications* base)
  {
      m_base=base;
      //printf("PTR: %p (set_notify_base)\n", base);
  }
