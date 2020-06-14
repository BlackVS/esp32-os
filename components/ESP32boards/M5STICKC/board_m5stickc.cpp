#include "boards.h"
#include "board_m5stickc.h"

#include "esp_log.h"
#include "I2Cbus.hpp"
#include "esp32_wifi.h"
#include "esp32_tools.h"

#include "MPU.hpp" 

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
///// Board components:

//i2c bus
I2C_t i2c0 = i2cbus::I2C(I2C_NUM_0, M5STICKC::config::AXP192_SDA, M5STICKC::config::AXP192_SCL, M5STICKC::config::AXP192_CLK);

//attach AXP to i2c 
AXP192  axp(i2c0, M5STICKC::config::AXP192_ADDR);

//Attached MPU to i2c0
MPU_t MPU(i2c0, M5STICKC::config::MPU_I2C_ADDR); //More complex, supports few models of MPU, type of MPU set vis sdkconfig
//MPU6886 MPU(i2c0, M5STICKC::config::MPU_I2C_ADDR); //Simpler, taken from M5StickC Arduino library

//SPI bus
//M5STICKC 27000000
//                        Bus        MOSI/SDA     MISO         CLK/SCL
ESP32_SPIbus 	spiBus(HSPI_HOST, GPIO_NUM_15, GPIO_NUM_NC, GPIO_NUM_13); //M5STICKC

//SPI display device
//                                   Reset        CS           DC
ESP32_SPIdevice spiDevice(spiBus, GPIO_NUM_18, GPIO_NUM_5, GPIO_NUM_23, 40000000);

//initialize SPI TFT
//                                 W   H  ofsX ofsY
ESP32_SPI_TFT   spiTFT(spiDevice, 80, 160, 26,  1,  TFT_ROTATION_90, TFT_ST7735_M5STICKC);

//attach graphics engine to TFT
DISPLAY_TYPE    display(spiTFT);



Board_M5STICKC::Board_M5STICKC()
{
    //delay(100); for breakpoints only
}


void Board_M5STICKC::init() 
{
    ESP_LOGD(TAG,"%s: starting", __FUNCTION__);
    
    axp.init();
    mpu_init(MPU);


    ESP_LOGD(__FUNCTION__, "Starting...");
    // Initialize I2C on port 0 using I2Cbus interface
    WiFi.init();

    //Display init
    ESP_LOGD(__FUNCTION__, "Starting...");
    display.begin(); // will raise error on i2c init - it is normal
    display.setFixedFont(ssd1306xled_font6x8);
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
    delay(2000);

}
