#pragma once

#pragma once

#include <driver/gpio.h>
#include <math.h>

#include "MPU.hpp"        // main file, provides the class itself
#include "mpu/math.hpp"   // math helper for dealing with MPU data
#include "mpu/types.hpp"  // MPU data types and definitions
#include "I2Cbus.hpp"
// ...
//i2c0.begin(SDA, SCL, CLOCK);  // initialize the I2C bus


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

extern TaskHandle_t mpu_task_handle;

void mpu_task(void *pvParameters);

// should be called only once
void mpu_task_init(void);
void mpu_task_finish(void);

//protected by mutex
int MPU_get(CMPUData& data);
