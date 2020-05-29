#include "app.h"

#ifdef BOARD_TTGO

static const char *TAG = __FILE__;
Board_TTGO board=Board_TTGO();
//////////////////////////////////////////////////////////////////////////

/// Board compomnents:

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

// DSIPALY
// 1. If defined busID - apply HSPI/VSPI defaults first
// 2. MISO: -1 default, 0 - don't use


//TTGO test connection
//                               RST busID CS  DC freq     SCL SDA  MISO
//DISPLAY_TYPE display(18, {-1,   5, 23, 0,  19, 22, 0});
//DISPLAY_TYPE display(128, 128, 18, {-1,   5, 23, 0,  19, 22, 0});
DISPLAY_TYPE   display(80,  160, 18, {-1,   5, 23, 27000000,  19, 22, 0} ST7735_M5STICKC); //test TTGO 80x160

//DisplaySSD1306_128x64_I2C display(-1, {I2C_NUM_0, OLED_I2C_ADDR, OLED_I2C_SCL_IO, OLED_I2C_SDA_IO, OLED_I2C_FREQ_HZ} ); // {busId, addr, scl, sda, freq}
//DisplayST7735_80x160x16_SPI display(18, {-1, 5, 23, 0, 13, 15});
//DisplayST7735_128x160x16_SPI display(18, {-1, 5, 23, 0, 13, 15});

//DisplaySSD1306_128x64_SPI display(-1,{-1, 0, 1, 0, -1, -1); // Use this line for nano pi (RST not used, 0=CE, gpio1=D/C)
//DisplaySSD1306_128x64_SPI display(3,{-1, 4, 5, 0,-1,-1});   // Use this line for Atmega328p (3=RST, 4=CE, 5=D/C)
//DisplaySSD1306_128x64_SPI display(24,{-1, 0, 23, 0,-1,-1}); // Use this line for Raspberry  (gpio24=RST, 0=CE, gpio23=D/C)
//DisplaySSD1306_128x64_SPI display(22,{-1, 5, 21, 0,-1,-1}); // Use this line for ESP32 (VSPI)  (gpio22=RST, gpio5=CE for VSPI, gpio21=D/C)

//AXP192       axp(M5STICKC::config::AXP192_SDA, M5STICKC::config::AXP192_SCL, M5STICKC::config::AXP192_CLK, M5STICKC::config::AXP192_ADDR);



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
    display.setOffset(26,1);
    display.setFixedFont(ssd1306xled_font6x8);
    display.clear();
    uint32_t x=0;
    uint32_t y=0;
    display.drawRect(0,0, display.width()-1, display.height()-1);
    display.setColor(0xf00f);
    display.printFixed(x, y+=8, "===============", STYLE_NORMAL);
    display.printFixed(x, y+=8, " ESP OS by VVS ", STYLE_BOLD);
    display.printFixed(x, y+=8, "===============", STYLE_NORMAL);
    display.printFixed(x, y+=8, "  version " FIRMWARE_VERSION, STYLE_ITALIC);
    delay(2000);

    ESP_LOGD(TAG,"%s: started", __FUNCTION__);
}


void Board_TTGO::run() 
{
  ESP_LOGD(TAG, "%s", __FUNCTION__);

}


#endif