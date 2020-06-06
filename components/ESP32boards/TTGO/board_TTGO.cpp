
#include "boards.h"
#include "board_TTGO.h"

#include "esp32_wifi.h"
#include "esp_log.h"

static const char *TAG = __FILE__;
Board_TTGO board=Board_TTGO();
//////////////////////////////////////////////////////////////////////////

/// Board components:

// ESP32 SPI
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/spi_master.html

// By default, the pin mapping for SPI is:
//        SPI2  SPI3
//        HSPI  VSPI
// CS0*    15    5
// SCLK    14    18   SCL
// MISO    12    19
// MOSI    13    23   SDA
// QUADWP   2    22
// QUADHD   4    21

// *Only the first Device attached to the bus can use the CS0 pin

//busID
//#define HSPI_HOST   1
//#define VSPI_HOST   2
//-1 = default for platform, VSPI_HOST for esp32


// SPI bus - we use HSPI host on M5STICKC like ports  
//ESP32_SPIbus 	spiBus(HSPI_HOST, GPIO_NUM_15, GPIO_NUM_NC, GPIO_NUM_13); //M5STICKC like
//                        Bus       MOSI/SDA     MISO         CLK/SCL

// SPI bus - little bit different from M5STICKC to enable JTAG - 13,15 -> 19,22
ESP32_SPIbus 	spiBus(HSPI_HOST, GPIO_NUM_22, GPIO_NUM_NC, GPIO_NUM_19); //+ JTAG
//                        Bus      MOSI/SDA     MISO         CLK/SCL

// Control pins for TFT - reset, select, data control
ESP32_SPIdevice spiDevice(spiBus, GPIO_NUM_18, GPIO_NUM_5, GPIO_NUM_23, 27000000);
//                                   Reset        CS           DC

// Initialize 80*160*16 TFT 
//ESP32_SPI_TFT   spiTFT(spiDevice, 80, 160, 26,  1,  TFT_ROTATION_90,  TFT_ST7735_M5STICKC); 80*160*16 TFT 0.96 BGR
ESP32_SPI_TFT   spiTFT(spiDevice, 128, 128, 2,  1,  TFT_ROTATION_0,  TFT_ST7735_128x128x16);
//                                 W   H  ofsX ofsY

// And attch to NanoDisplay library
DISPLAY_TYPE    display(spiTFT);


Board_TTGO::Board_TTGO()
{
    //delay(100); for breakpoints only
}


void Board_TTGO::init() 
{
    ESP_LOGD(TAG,"%s: starting", __FUNCTION__);

    // Initialize I2C on port 0 using I2Cbus interface
    WiFi.init();

    //Display init
    ESP_LOGD(__FUNCTION__, "Starting...");
    display.begin(); // will raise error on i2c init - it is normal


    //Test
    display.setFixedFont(ssd1306xled_font6x8);
    //display.setFreeFont( free_calibri11x12, free_calibri11x12_cyrillic );

    display.clear();
    display.drawRect(0,0, display.width()-1, display.height()-1 );
    display.setColor(0xf00f);
    uint32_t dy=0;
    uint32_t dx=display.getFont().getTextSize("==========", &dy);
    
    uint32_t x=(display.width()-dx)/2;
    uint32_t y=(display.height()-5*8)/2-8;

    display.printFixed(x, y+=8, "==========", STYLE_NORMAL);
    display.printFixed(x, y+=8, "  ESP OS  ", STYLE_BOLD);
    display.printFixed(x, y+=8, "  by VVS  ", STYLE_BOLD);
    display.printFixed(x, y+=8, "==========", STYLE_NORMAL);
    display.printFixed(x, y+=8, "  v. " FIRMWARE_VERSION, STYLE_ITALIC);
    lcd_delay(2000);

}
