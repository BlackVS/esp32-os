#pragma once

#include <driver/rmt.h>
#include <driver/touch_pad.h>

//#define NNC2019_JTAG_ENABLED 1

namespace NNC2019 {
    namespace config{
        //LEDS Strip
        constexpr uint32_t      LEDS_NUM=10;
        constexpr gpio_num_t    LEDS_PIN=GPIO_NUM_4;
        constexpr rmt_channel_t LEDS_RMT_CHANNEL=RMT_CHANNEL_0;

        //blue led on badge
        constexpr gpio_num_t    PIN_LED_BLUE=GPIO_NUM_5; 

        //touchpad
        constexpr touch_pad_t   PIN_TOUCH=TOUCH_PAD_NUM5;  /*!< Touch pad channel 5 is GPIO12*/

        //i2c0
        constexpr gpio_num_t    IIC0_SCL_PIN = GPIO_NUM_16;
        constexpr gpio_num_t    IIC0_SDA_PIN = GPIO_NUM_17;
        constexpr uint32_t      IIC0_FREQ    = 400000; //100000 or 400000

        //display SSD1306
        //uses i2c0
        constexpr uint32_t      OLED_I2C_ADDR=0x3c;

        //mpu
        //uses i2c0
        constexpr uint8_t       MPU_I2C_ADDR = 0x68; //MPU_I2CADDRESS_AD0_LOW
    }
}


// //CAN
// #define PIN_CAN_RX GPIO_NUM_21
// #define PIN_CAN_TX GPIO_NUM_22


// //adc
// #define PIN_ADC1 GPIO_NUM_34
// // #define PIN_ADC2 33

// //buzzer
// #define PIN_BUZZER GPIO_NUM_13

// // PIN 12 == T5

// //leds

// #define BLINK_GPIO GPIO_NUM_2



//#define MPU_INT_IO GPIO_NUM_27		// MPU Interrupt pin GPIO number

