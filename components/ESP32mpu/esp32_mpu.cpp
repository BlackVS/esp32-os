#include "esp32_mpu.h"


// #include "MPU.hpp"        // main file, provides the class itself
//#include "mpu/math.hpp"   // math helper for dealing with MPU data
// #include "mpu/types.hpp"  // MPU data types and definitions

#define UNUSED __attribute__ ((unused)) 

static const char *TAG = "MPU";

TaskHandle_t mpu_task_handle = NULL;

static SemaphoreHandle_t mpu_data_mutex = NULL;
static CMPUData mpu_data;

int MPU_set(const CMPUData& data)
{
	if( mpu_data_mutex )
   	{
	   //ESP_LOGD(TAG,"%s : mpu_data_mutex exists", __FUNCTION__);
       // See if we can obtain the semaphore.  If the semaphore is not available
       // wait 10 ticks to see if it becomes free.
       if( xSemaphoreTake( mpu_data_mutex, ( TickType_t ) 10 ) == pdTRUE )
       {
	   	   //ESP_LOGD(TAG,"%s : mpu_data_mutex taken", __FUNCTION__);
           // We were able to obtain the semaphore and can now access the
           // shared resource.
		   mpu_data = data;
           // We have finished accessing the shared resource.  Release the
           // semaphore.
           xSemaphoreGive( mpu_data_mutex );
	   	   //ESP_LOGD(TAG,"%s : mpu_data_mutex given", __FUNCTION__);
		   return 0;
       }
       else
       {
           // We could not obtain the semaphore and can therefore not access
           // the shared resource safely.
	   	   //ESP_LOGD(TAG,"%s : mpu_data_mutex busy", __FUNCTION__);
		   return -2;
       }
    }
    //ESP_LOGD(TAG,"%s : mpu_data_mutex not inited", __FUNCTION__);
	return -1;	
}

int MPU_get(CMPUData& data)
{
	if( mpu_data_mutex )
   	{
       // See if we can obtain the semaphore.  If the semaphore is not available
       // wait 10 ticks to see if it becomes free.
       if( xSemaphoreTake( mpu_data_mutex, ( TickType_t ) 10 ) == pdTRUE )
       {
           // We were able to obtain the semaphore and can now access the
           // shared resource.
	   	   data = mpu_data;
           // We have finished accessing the shared resource.  Release the
           // semaphore.
           xSemaphoreGive( mpu_data_mutex );
		   return 0;
       }
       else
       {
           // We could not obtain the semaphore and can therefore not access
           // the shared resource safely.
		   return -2;
       }
    }
	return -1;	
}



uint8_t mpu_accelFSRvalue(const MPU_ACCEL_FS fs)
{
    return 2 << fs;
}

float mpu_accelResolution(const MPU_ACCEL_FS fs)
{
    return static_cast<float>(mpu_accelFSRvalue(fs)) / INT16_MAX;
}

MPU_AXES_FLOAT mpu_accelGravity(const MPU_AXES_RAW& raw_axes, const MPU_ACCEL_FS fs)
{
    MPU_AXES_FLOAT axes;
    axes.x = raw_axes.x * mpu_accelResolution(fs);
    axes.y = raw_axes.y * mpu_accelResolution(fs);
    axes.z = raw_axes.z * mpu_accelResolution(fs);
    return axes;
}

uint16_t mpu_gyroFSRvalue(const MPU_GYRO_FS fs)
{
    return 250 << fs;
}

float mpu_gyroResolution(const MPU_GYRO_FS fs)
{
    return static_cast<float>(mpu_gyroFSRvalue(fs)) / INT16_MAX;
}


float mpu_gyroDegPerSec(const int16_t axis, const MPU_GYRO_FS fs)
{
    return axis * mpu_gyroResolution(fs);
}


MPU_AXES_FLOAT mpu_gyroDegPerSec(const MPU_AXES_RAW& raw_axes, const MPU_GYRO_FS fs)
{
    MPU_AXES_FLOAT axes;
    axes.x = raw_axes.x * mpu_gyroResolution(fs);
    axes.y = raw_axes.y * mpu_gyroResolution(fs);
    axes.z = raw_axes.z * mpu_gyroResolution(fs);
    return axes;
}



void mpu_task_init(void) 
{
	ESP_LOGD(__FUNCTION__, "Starting...");
	mpu_data_mutex = xSemaphoreCreateBinary();
	xSemaphoreGive( mpu_data_mutex );
	ESP_LOGD(__FUNCTION__, "Successfully started...");
}

void mpu_task_finish(void)
{
	if(mpu_data_mutex){
		vSemaphoreDelete(mpu_data_mutex);
		mpu_data_mutex=NULL;
	}
}
