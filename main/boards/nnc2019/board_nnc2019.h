#pragma once

#include "board_nnc2019_config.h"
#include "blink.h"

typedef DisplaySSD1306_128x64_CustomI2C<I2C_LCDGFX> DISPLAY_TYPE;

class CLEDStrip;

class Board_NNC2019 : public Board
{
    public:
        Board_NNC2019();
        virtual ~Board_NNC2019() {}

    public:
        virtual void init();
        virtual void run();

};

void self_test_task(void *pvParameters) ;

extern Board_NNC2019 board;
extern DISPLAY_TYPE  display;
extern CLEDStrip     leds;
