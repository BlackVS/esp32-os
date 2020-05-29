#pragma once

#ifdef BOARD_M5STICKC

#include "board_m5stickc_config.h"
#include "AXP192.h"
//#include "axp192.h"

typedef ESP32_Display DISPLAY_TYPE;

class Board_M5STICKC : public Board
{
    public:
        Board_M5STICKC();
        virtual ~Board_M5STICKC() {}

    public:
        virtual void init();
        virtual void run();

};

extern Board_M5STICKC board;
extern DISPLAY_TYPE   display;


#endif