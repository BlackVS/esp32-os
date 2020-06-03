#pragma once

#include "board_config.h"
#include "board_interface.h"

#if defined(CONFIG_ESP32_BOARD_NNC2019)
    #include "board_nnc2019.h"
#elif defined(CONFIG_ESP32_BOARD_M5STICKC)
    #include "board_m5stickc.h"
#elif defined(CONFIG_ESP32_BOARD_TTGO)
    #include "board_TTGO.h"
#endif