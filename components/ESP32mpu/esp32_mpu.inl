template<class MPUDRIVER>
void mpu_task(void *pvParameters) 
{
	if(!pvParameters) {
        ESP_LOGE("MPU", "Zero pointer instead MPU!");
        vTaskDelete(NULL);
        return;
    }
    MPUDRIVER* MPU=(MPUDRIVER*)pvParameters;

    float roll{0}, pitch{0}, yaw{0};


    // Great! Let's verify the communication
    // (this also check if the connected MPU supports the implementation of chip selected in the component menu)
    while (esp_err_t err = MPU->testConnection()) {
        ESP_LOGE("MPU", "Failed to connect to the MPU, error=%#X", err);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI("MPU", "MPU connection successful!");

    // Initialize
    ESP_ERROR_CHECK(MPU->initialize());  // initialize the chip and set initial configurations

    // Setup with your configurations
    // ESP_ERROR_CHECK(MPU.setSampleRate(50));  // set sample rate to 50 Hz
    
    MPU->setGyroFullScale(1);//GYRO_FS_500DPS
    MPU->setAccelFullScale(1);//MPU_ACCEL_FS_4G
    //MPU->setSampleRate(100);  // in (Hz)
    //MPU->setDigitalLowPassFilter(MPU_DLPF_42HZ);  // smoother data
    //MPU.setInterruptEnabled(mpud::INT_EN_RAWDATA_READY);  // enable INT pin


	ESP_LOGD("MPU", "Init successful!");

    // Reading sensor data
    //printf("Reading sensor data:\n");
    MPU_AXES_RAW   accelRaw;   // x, y, z axes as int16
    MPU_AXES_RAW   gyroRaw;    // x, y, z axes as int16
    MPU_AXES_FLOAT accelG;   // accel axes in (g) gravity format
    MPU_AXES_FLOAT gyroDPS;  // gyro axes in (DPS) º/s format

    const int msDelay = 100;
    const TickType_t xDelay = msDelay / portTICK_PERIOD_MS;

	while (1) 
    {
        // Read
        MPU->accelerationRaw(&accelRaw.x, &accelRaw.y, &accelRaw.z);  // fetch raw data from the registers
        MPU->rotationRaw(&gyroRaw.x, &gyroRaw.y, &gyroRaw.z);       // fetch raw data from the registers
        
        
        // float t=0;
        // MPU->temperatureC(&t);
        // printf("T=%f\n", t);

        // MPU.motion(&accelRaw, &gyroRaw);  // read both in one shot
        // Convert
        accelG =  mpu_accelGravity(accelRaw, MPU_ACCEL_FS_4G);
        gyroDPS = mpu_gyroDegPerSec(gyroRaw, MPU_GYRO_FS_500DPS);

        // Calculate tilt angle
        // range: (roll[-180,180]  pitch[-90,90]  yaw[-180,180])
        constexpr double kRadToDeg = 57.2957795131;
        constexpr float kDeltaTime = float(msDelay) / 1000.f;
        float gyroRoll             = roll  + mpu_gyroDegPerSec(gyroRaw.x, MPU_GYRO_FS_500DPS) * kDeltaTime;
        float gyroPitch            = pitch + mpu_gyroDegPerSec(gyroRaw.y, MPU_GYRO_FS_500DPS) * kDeltaTime;
        float gyroYaw              = yaw   + mpu_gyroDegPerSec(gyroRaw.z, MPU_GYRO_FS_500DPS) * kDeltaTime;
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
        // printf("accel: [%+6.2f %+6.2f %+6.2f ] (G) \t", accelG.x, accelG.y, accelG.z);
        // printf("gyro: [%+7.2f %+7.2f %+7.2f ] (º/s)\t", gyroDPS[0], gyroDPS[1], gyroDPS[2]);
        // printf("Pitch: %+6.1f \t Roll: %+6.1f \t Yaw: %+6.1f \n", pitch, roll, yaw);
        
        vTaskDelay(xDelay);
	}
    vTaskDelete(NULL);    
}

template<class MPUDRIVER>
void mpu_init(MPUDRIVER& MPU)
{
  mpu_task_init();

  //MPU: reads sensora
  xTaskCreate(&mpu_task<MPUDRIVER> , "MPU" , 4096, &MPU, 5, &mpu_task_handle);
}
