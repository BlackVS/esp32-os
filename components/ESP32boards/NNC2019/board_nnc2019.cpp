//#include "app.h"

#include "boards.h"
#include "board_nnc2019.h"

#include "esp_log.h"
#include "I2Cbus.hpp"
#include "esp32_wifi.h"
#include "led_strip.h"


Board_NNC2019 board=Board_NNC2019();


I2C_t i2c0 = i2cbus::I2C(I2C_NUM_0, NNC2019::config::IIC0_SDA_PIN, NNC2019::config::IIC0_SCL_PIN, NNC2019::config::IIC0_FREQ);

//Attached OLED at i2c0
ESP32_I2Cdevice i2c_disp(i2c0, NNC2019::config::OLED_I2C_ADDR);
ESP32_I2C_TFT   i2cTFT(i2c_disp, 128, 64, TFT_SSD1306_128x64x1);

//Attached MPU at i2c0
MPU_t MPU(i2c0, NNC2019::config::MPU_I2C_ADDR);


DISPLAY_TYPE    display(i2cTFT);
//ESP32_Display<NanoDisplayOps1, ESP32_I2C_TFT> display(i2cTFT);

CBlink       blue_led(NNC2019::config::PIN_LED_BLUE);
CLEDStrip    leds(NNC2019::config::LEDS_NUM, NNC2019::config::LEDS_RMT_CHANNEL, NNC2019::config::LEDS_PIN );
#ifndef NNC2019_JTAG_ENABLED
CTouchButton touchpad(NNC2019::config::PIN_TOUCH);
#else
CTouchButtonDummy touchpad;
#endif

Board_NNC2019::Board_NNC2019()
{
    //delay(100); for breakpoints only
}


void Board_NNC2019::init() 
{
    // Initialize I2C on port 0 using I2Cbus interface
    i2c0.begin();
    //
    WiFi.init();
    mpu_init(MPU);
    blue_led.init();
    leds.init();
    touchpad.init();

    //Display init
    ESP_LOGD(__FUNCTION__, "Starting...");
    display.begin(); // will raise error on i2c init - it is normal
    display.setFixedFont(ssd1306xled_font6x8);
    display.clear();
    uint32_t x=16;
    uint32_t y=8;
    display.printFixed(x, y+=8, "===============", STYLE_NORMAL);
    display.printFixed(x, y+=8, " ESP OS by VVS ", STYLE_BOLD);
    display.printFixed(x, y+=8, "===============", STYLE_NORMAL);
    display.printFixed(x, y+=8, "  version " FIRMWARE_VERSION, STYLE_ITALIC);
    delay(2000);
    // display.tft().display_off();
    // delay(2000);
    // display.tft().display_on();
    // delay(2000);
    // display.tft().display_flip_horizontal(true);
    // display.tft().display_flip_vertical(true);
    // delay(2000);
    // display.tft().display_flip_vertical(false);
    // display.tft().display_flip_horizontal(false);

    ESP_LOGD(__FUNCTION__, "Successfully started...");
}


