#pragma once

#define FIRMWARE_VERSION "0.1"

#define CONFIG_JTAG_ENABLED 1




//Choose only one of!!!
//#define BADGE_ASTEROIDS
//#define BADGE_CORONA
//#define BADGE_LEDS1
#define BADGE_LEDS2


#define NUM_LEDS 10

//blue led on badge
#define PIN_LED_BLUE GPIO_NUM_5
//#define GPIO_OUTPUT_PIN_SEL (1ULL << PIN_LED_BLUE)

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
#define PIN_WS2812 GPIO_NUM_4

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

//touch pad
#define PIN_TOUCH TOUCH_PAD_NUM5
#define TOUCHPAD_NO_CHANGE (-1)
#define TOUCHPAD_THRESH_NO_USE (0)
#define TOUCHPAD_FILTER_MODE
#define TOUCHPAD_FILTER_TOUCH_PERIOD (10)
#define TOUCHPAD_TRESH 600
