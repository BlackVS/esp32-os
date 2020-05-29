#include "app.h"

#ifdef BOARD_M5STICKC


#define SCREEN_WIDTH	80
#define SCREEN_HEIGHT	160
#define	INTERVAL	100

#define WAIT	vTaskDelay(INTERVAL)


// #define TFT_MOSI      15
// #define TFT_SCLK      13
// #define TFT_CS        5   // Chip select line for TFT display on Shield
// #define TFT_DC        23  // Data/command line for TFT on Shield
// #define TFT_RST       18  // Reset line for TFT is handled by seesaw!

// // #define SPI_FREQUENCY   1000000
// // #define SPI_FREQUENCY   5000000
// // #define SPI_FREQUENCY  10000000
// // #define SPI_FREQUENCY  20000000
// #define SPI_FREQUENCY  27000000 // Actually sets it to 26.67MHz = 80/3
// // #define SPI_FREQUENCY  40000000 // Maximum to use SPIFFS
// // #define SPI_FREQUENCY  80000000

// // Optional reduced SPI frequency for reading TFT
// #define SPI_READ_FREQUENCY  20000000

// // The XPT2046 requires a lower SPI clock rate of 2.5MHz so we define that here:
// #define SPI_TOUCH_FREQUENCY  2500000

static const char *TAG = __FILE__;
Board_M5STICKC board=Board_M5STICKC();
//////////////////////////////////////////////////////////////////////////

/// Board compomnents:
//M5STICKC 27000000
//                        Bus        MOSI/SDA     MISO         CLK/SCL
ESP32_SPIbus 	spiBus(HSPI_HOST, GPIO_NUM_15, GPIO_NUM_NC, GPIO_NUM_13); //M5STICKC
//ESP32_SPIbus 	spiBus(HSPI_HOST, GPIO_NUM_22, GPIO_NUM_NC, GPIO_NUM_19); //+ JTAG

//                                   Reset        CS           DC
ESP32_SPIdevice spiDevice(spiBus, GPIO_NUM_18, GPIO_NUM_5, GPIO_NUM_23, 40000000);
//                                 W   H  ofsX ofsY
ESP32_SPI_TFT   spiTFT(spiDevice, 80, 160, 26,  1,   TFT_ST7735_M5STICKC);
DISPLAY_TYPE    display(spiTFT);


//                           RST busID CS DC  freq       SCL SDA
//DISPLAY_TYPE display(80, 160, 18, {0,  5, 23, 27000000,  13, 15,  0},   ST7735_M5STICKC); //M5STICKC
//DISPLAY_TYPE display(18, {-1, 5, 23, 0, 13, 15});
AXP192       axp(M5STICKC::config::AXP192_SDA, M5STICKC::config::AXP192_SCL, M5STICKC::config::AXP192_CLK, M5STICKC::config::AXP192_ADDR);


Board_M5STICKC::Board_M5STICKC()
{
    //delay(100); for breakpoints only
}


void Board_M5STICKC::init() 
{
    ESP_LOGD(TAG,"%s: starting", __FUNCTION__);
    
    //axp.init();

    ESP_LOGD(__FUNCTION__, "Starting...");
	//combine these three in one display.begin() call
	spiBus.begin();
	spiDevice.begin();
	//spiTFT.begin(); -> inside display.begin()

    //Display init
    display.begin(); // will raise error on i2c init - it is normal
    display.setOffset(26,1);
    display.clear();
    display.drawRect(0,0, display.width()-1, display.height()-1 );
    display.setColor(0xf00f);
    display.setFixedFont(ssd1306xled_font6x8);

    uint32_t x=12;
    uint32_t y=8;
    display.printFixed(x, y+=8, "==========", STYLE_NORMAL);
    display.printFixed(x, y+=8, "  ESP OS  ", STYLE_BOLD);
    display.printFixed(x, y+=8, "  by VVS  ", STYLE_BOLD);
    display.printFixed(x, y+=8, "==========", STYLE_NORMAL);
    display.printFixed(x, y+=8, " ver. " FIRMWARE_VERSION, STYLE_ITALIC);
    delay(2000);


    ESP_LOGD(TAG,"%s: started", __FUNCTION__);
}


void Board_M5STICKC::run() 
{
  ESP_LOGD(TAG, "%s", __FUNCTION__);

}


#endif