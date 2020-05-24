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

    }
}


//CAN
#define PIN_CAN_RX GPIO_NUM_21
#define PIN_CAN_TX GPIO_NUM_22

//i2c
#define PIN_IIC_SCL GPIO_NUM_16
#define PIN_IIC_SDA GPIO_NUM_17

//adc
#define PIN_ADC1 GPIO_NUM_34
// #define PIN_ADC2 33

//buzzer
#define PIN_BUZZER GPIO_NUM_13

// PIN 12 == T5

//leds

#define BLINK_GPIO GPIO_NUM_2

//display SSD1306
#define OLED_I2C_SCL_IO PIN_IIC_SCL  
#define OLED_I2C_SDA_IO PIN_IIC_SDA  
#define OLED_I2C_ADDR   0x3c
#define OLED_I2C_FREQ_HZ 100000

//mpu
#define MPU_I2C_SCL_IO PIN_IIC_SCL      
#define MPU_I2C_SDA_IO PIN_IIC_SDA  
#define MPU_I2C_FREQ_HZ 100000		// I2C clock frequency

//#define MPU_INT_IO GPIO_NUM_27		// MPU Interrupt pin GPIO number



