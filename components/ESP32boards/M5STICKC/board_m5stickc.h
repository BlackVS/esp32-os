#pragma once



#include "board_m5stickc_config.h"
#include "board_interface.h"
#include "esp32_tft.h"
#include "esp32_tft_spi.h"

#include "esp32_mpu.h"
#include "MPU6886.h"
#include "AXP192.h"
//#include "axp192.h"

typedef ESP32_Display<16, ESP32_SPI_TFT> DISPLAY_TYPE;
typedef NanoEngine16<DISPLAY_TYPE> NANO_ENGINE_TYPE;
typedef NanoFixedSprite<NANO_ENGINE_TYPE::TilerT> NANO_SPRITE_TYPE;

class Board_M5STICKC : public Board
{
    public:
        Board_M5STICKC();
        virtual ~Board_M5STICKC() {}

    public:
        virtual void init();
};

extern Board_M5STICKC board;
extern DISPLAY_TYPE   display;
