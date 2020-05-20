#include "app.h"


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


void mpu_task(void *arg) 
{
	float roll{0}, pitch{0}, yaw{0};

	MPU_t MPU;  // create a default MPU object
    MPU.setBus(i2c0);  // set bus port, not really needed since default is i2c0
    MPU.setAddr(mpud::MPU_I2CADDRESS_AD0_LOW);  // set address, default is AD0_LOW

    // Great! Let's verify the communication
    // (this also check if the connected MPU supports the implementation of chip selected in the component menu)
    while (esp_err_t err = MPU.testConnection()) {
        ESP_LOGE(TAG, "Failed to connect to the MPU, error=%#X", err);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "MPU connection successful!");

    // Initialize
    ESP_ERROR_CHECK(MPU.initialize());  // initialize the chip and set initial configurations
    static constexpr mpud::gyro_fs_t kGyroFS   = mpud::GYRO_FS_500DPS;

    // Setup with your configurations
    // ESP_ERROR_CHECK(MPU.setSampleRate(50));  // set sample rate to 50 Hz
    ESP_ERROR_CHECK(MPU.setGyroFullScale(kGyroFS));
    // ESP_ERROR_CHECK(MPU.setAccelFullScale(mpud::ACCEL_FS_4G));


	ESP_LOGD(TAG, "Init successful!");

    // Reading sensor data
    //printf("Reading sensor data:\n");
    mpud::raw_axes_t accelRaw;   // x, y, z axes as int16
    mpud::raw_axes_t gyroRaw;    // x, y, z axes as int16
    mpud::float_axes_t accelG;   // accel axes in (g) gravity format
    mpud::float_axes_t gyroDPS;  // gyro axes in (DPS) º/s format

    const int msDelay = 100;
    const TickType_t xDelay = msDelay / portTICK_PERIOD_MS;

	while (1) {
// Read
        MPU.acceleration(&accelRaw);  // fetch raw data from the registers
        MPU.rotation(&gyroRaw);       // fetch raw data from the registers
        // MPU.motion(&accelRaw, &gyroRaw);  // read both in one shot
        // Convert
        accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_4G);
        gyroDPS = mpud::gyroDegPerSec(gyroRaw, mpud::GYRO_FS_500DPS);

        // Calculate tilt angle
        // range: (roll[-180,180]  pitch[-90,90]  yaw[-180,180])
        constexpr double kRadToDeg = 57.2957795131;
        constexpr float kDeltaTime = float(msDelay) / 1000.f;
        float gyroRoll             = roll + mpud::math::gyroDegPerSec(gyroRaw.x, kGyroFS) * kDeltaTime;
        float gyroPitch            = pitch + mpud::math::gyroDegPerSec(gyroRaw.y, kGyroFS) * kDeltaTime;
        float gyroYaw              = yaw + mpud::math::gyroDegPerSec(gyroRaw.z, kGyroFS) * kDeltaTime;
        float accelRoll            = atan2(-accelRaw.x, accelRaw.z) * kRadToDeg;
        float accelPitch = atan2(accelRaw.y, sqrt(accelRaw.x * accelRaw.x + accelRaw.z * accelRaw.z)) * kRadToDeg;
        // Fusion
        roll  = gyroRoll * 0.95f + accelRoll * 0.05f;
        pitch = gyroPitch * 0.95f + accelPitch * 0.05f;
        yaw   = gyroYaw;
        // correct yaw
        if (yaw > 180.f)
            yaw -= 360.f;
        else if (yaw < -180.f)
            yaw += 360.f;

		CMPUData data;
		data.accelG[0]=accelG.x;
		data.accelG[1]=accelG.y;
		data.accelG[2]=accelG.z;
		data.gyroDPS[0]=gyroDPS[0];
		data.gyroDPS[1]=gyroDPS[1];
		data.gyroDPS[2]=gyroDPS[2];
		data.pitch=pitch;
		data.roll=roll;
		data.yaw=yaw;
		data.empty=false;
		MPU_set(data);

        // Debug
        //printf("accel: [%+6.2f %+6.2f %+6.2f ] (G) \t", accelG.x, accelG.y, accelG.z);
        //printf("gyro: [%+7.2f %+7.2f %+7.2f ] (º/s)\t", gyroDPS[0], gyroDPS[1], gyroDPS[2]);
        //printf("Pitch: %+6.1f \t Roll: %+6.1f \t Yaw: %+6.1f \n", pitch, roll, yaw);
        
        vTaskDelay(xDelay);
	}
}

