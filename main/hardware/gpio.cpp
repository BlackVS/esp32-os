#include "app.h"

void i2c_gpio_init(void)
{
    // Initialize I2C on port 0 using I2Cbus interface
    i2c0.setup(MPU_I2C_SDA_IO, MPU_I2C_SCL_IO, MPU_I2C_FREQ_HZ);
    i2c0.begin();

    // Or directly with esp-idf API
    /*
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = MPU_I2C_SDA_IO;
    conf.scl_io_num = MPU_I2C_SCL_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = MPU_I2C_FREQ_HZ;
    ESP_ERROR_CHECK(i2c_param_config(MPU_I2C_PORT, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(MPU_I2C_PORT, conf.mode, 0, 0, 0));
    */

}

void gpio_init(void)
{
    #ifdef BOARD_BADGE_NNC2019
    i2c_gpio_init(); 
    #endif
}