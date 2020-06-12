/*
 Note: The MPU6886 is an I2C sensor and uses the Arduino Wire library.
 Because the sensor is not 5V tolerant, we are using a 3.3 V 8 MHz Pro Mini or
 a 3.3 V Teensy 3.1. We have disabled the internal pull-ups used by the Wire
 library in the Wire.h/twi.c utility file. We are also using the 400 kHz fast
 I2C mode by setting the TWI_FREQ  to 400000L /twi.h utility file.
 */
#pragma once
#include "MahonyAHRS.h"
#include <cstdint>
#include "esp32_i2c.h"

#define MPU6886_ADDRESS           0x68 
#define MPU6886_WHOAMI            0x75
#define MPU6886_ACCEL_INTEL_CTRL  0x69
#define MPU6886_SMPLRT_DIV        0x19
#define MPU6886_INT_PIN_CFG       0x37
#define MPU6886_INT_ENABLE        0x38
#define MPU6886_ACCEL_XOUT_H      0x3B
#define MPU6886_ACCEL_XOUT_L      0x3C
#define MPU6886_ACCEL_YOUT_H      0x3D
#define MPU6886_ACCEL_YOUT_L      0x3E
#define MPU6886_ACCEL_ZOUT_H      0x3F
#define MPU6886_ACCEL_ZOUT_L      0x40

#define MPU6886_TEMP_OUT_H        0x41
#define MPU6886_TEMP_OUT_L        0x42

#define MPU6886_GYRO_XOUT_H       0x43
#define MPU6886_GYRO_XOUT_L       0x44
#define MPU6886_GYRO_YOUT_H       0x45
#define MPU6886_GYRO_YOUT_L       0x46
#define MPU6886_GYRO_ZOUT_H       0x47
#define MPU6886_GYRO_ZOUT_L       0x48

#define MPU6886_USER_CTRL         0x6A
#define MPU6886_PWR_MGMT_1        0x6B
#define MPU6886_PWR_MGMT_2        0x6C
#define MPU6886_CONFIG            0x1A
#define MPU6886_GYRO_CONFIG       0x1B
#define MPU6886_ACCEL_CONFIG      0x1C
#define MPU6886_ACCEL_CONFIG2     0x1D
#define MPU6886_FIFO_EN           0x23

//#define G (9.8)
#define RtA     57.324841
#define AtR    	0.0174533	
#define Gyro_Gr	0.0010653

class MPU6886 {

        I2C_t&      m_port;
        uint16_t    m_dev_addr = {0};

    public:
      enum Ascale 
      {
        ACCEL_FS_2G  = 0,  //!< +/- 2 g  -> 16.384 LSB/g
        ACCEL_FS_4G  = 1,  //!< +/- 4 g  -> 8.192 LSB/g
        ACCEL_FS_8G  = 2,  //!< +/- 8 g  -> 4.096 LSB/g
        ACCEL_FS_16G = 3   //!< +/- 16 g -> 2.048 LSB/g
      };

      enum Gscale {
        GYRO_FS_250DPS  = 0,  //!< +/- 250 º/s  -> 131 LSB/(º/s)
        GYRO_FS_500DPS  = 1,  //!< +/- 500 º/s  -> 65.5 LSB/(º/s)
        GYRO_FS_1000DPS = 2,  //!< +/- 1000 º/s -> 32.8 LSB/(º/s)
        GYRO_FS_2000DPS = 3   //!< +/- 2000 º/s -> 16.4 LSB/(º/s)
      };

      Gscale Gyscale = GYRO_FS_2000DPS;
      Ascale Acscale = ACCEL_FS_8G;
    public:
      MPU6886(I2C_t& port, uint16_t dev_addr);
      int initialize(void);

      //raw
      void accelerationRaw(int16_t* ax, int16_t* ay, int16_t* az);
      void rotationRaw(int16_t* gx, int16_t* gy, int16_t* gz);
      void temperatureRaw(int16_t *t);

      //converted to real
      void acceleration(float* ax, float* ay, float* az);
      void rotation(float* gx, float* gy, float* gz);
      void temperatureC(float *t);

      void setGyroFullScale(Gscale scale);
      void setGyroFullScale(int scale) { setGyroFullScale( (Gscale)scale); }
      void setAccelFullScale(Ascale scale);
      void setAccelFullScale(int scale) { setAccelFullScale((Ascale)scale); } 

      void getAhrsData(float *pitch,float *roll,float *yaw);
      //void getAhrsData(float *pitch,float *roll,float *yaw){


    public:
      uint8_t whoAmI();
      esp_err_t testConnection();

    public:
      float aRes, gRes;

    private:

    private:
      void I2C_Read_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *read_Buffer);
      void I2C_Write_NBytes(uint8_t driver_Addr, uint8_t start_Addr, uint8_t number_Bytes, uint8_t *write_Buffer);
      void getGres();
      void getAres();

};
