#pragma once

#include "board_nnc2019_config.h"
#include "blink.h"

typedef DisplaySSD1306_128x64_CustomI2C<I2C_LCDGFX> DISPLAY_TYPE;

class CLEDStrip;
class CTouchButton;

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

#ifndef NNC2019_JTAG_ENABLED
extern CTouchButton  touchpad;
#else
class CTouchButtonDummy
{
  public:
    CTouchButtonDummy() {}
    ~CTouchButtonDummy() {}
    void init() {}
    void run() {}
    TOUCHPAD_STATE get_state(void) { return TOUCHPAD_STATE_DISABLED; }
};
extern CTouchButtonDummy  touchpad;
#endif
