#pragma once

#include <cstdint>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "esp_log.h"
#include "esp_err.h"
#include <cmath>


// #include "MPU.hpp"        // main file, provides the class itself
// #include "mpu/math.hpp"   // math helper for dealing with MPU data
// #include "mpu/types.hpp"  // MPU data types and definitions
// #include "I2Cbus.hpp"
// ...
//i2c0.begin(SDA, SCL, CLOCK);  // initialize the I2C bus


/*! Gyroscope full-scale range */
typedef enum {
    MPU_GYRO_FS_250DPS  = 0,  //!< +/- 250 º/s  -> 131 LSB/(º/s)
    MPU_GYRO_FS_500DPS  = 1,  //!< +/- 500 º/s  -> 65.5 LSB/(º/s)
    MPU_GYRO_FS_1000DPS = 2,  //!< +/- 1000 º/s -> 32.8 LSB/(º/s)
    MPU_GYRO_FS_2000DPS = 3   //!< +/- 2000 º/s -> 16.4 LSB/(º/s)
} MPU_GYRO_FS;

/*! Accel full-scale range */
typedef enum {
    MPU_ACCEL_FS_2G  = 0,  //!< +/- 2 g  -> 16.384 LSB/g
    MPU_ACCEL_FS_4G  = 1,  //!< +/- 4 g  -> 8.192 LSB/g
    MPU_ACCEL_FS_8G  = 2,  //!< +/- 8 g  -> 4.096 LSB/g
    MPU_ACCEL_FS_16G = 3   //!< +/- 16 g -> 2.048 LSB/g
} MPU_ACCEL_FS;


/*! Generic axes struct to store sensors' data */
template <class type_t>
struct axes_t
{
    union
    {
        type_t xyz[3] = {0};
        struct
        {
            type_t x;
            type_t y;
            type_t z;
        };
    };
    type_t& operator[](int i) { return xyz[i]; }
};
// Ready-to-use axes types
typedef axes_t<int16_t> MPU_AXES_RAW  ; //!< Axes type to hold gyroscope, accelerometer, magnetometer raw data.
typedef axes_t<float>   MPU_AXES_FLOAT; 



class CMPUData {
public:	
	float accelG[3];     //x y z
	float gyroDPS[3];    //x y z
	float pitch, roll, yaw;
	bool  empty;
public:
	CMPUData(){
		empty=false;
	}
};

//protected by mutex
int MPU_get(CMPUData& data);
int MPU_set(const CMPUData& data);

template<class MPUDRIVER> 
void mpu_init(MPUDRIVER& MPU);


uint8_t 		mpu_accelFSRvalue(const MPU_ACCEL_FS fs);
float 			mpu_accelResolution(const MPU_ACCEL_FS fs);
MPU_AXES_FLOAT 	mpu_accelGravity(const MPU_AXES_RAW& raw_axes, const MPU_ACCEL_FS fs);
uint16_t 		mpu_gyroFSRvalue(const MPU_GYRO_FS fs);
float 			mpu_gyroResolution(const MPU_GYRO_FS fs);
float 			mpu_gyroDegPerSec(const int16_t axis, const MPU_GYRO_FS fs);
MPU_AXES_FLOAT 	mpu_gyroDegPerSec(const MPU_AXES_RAW& raw_axes, const MPU_GYRO_FS fs);


void mpu_task_init(void);
extern TaskHandle_t mpu_task_handle;
#include "esp32_mpu.inl"