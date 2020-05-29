#pragma once

#ifdef BOARD_TTGO

#include "board_TTGO_config.h"

//typedef DisplayST7735_128x128x16_SPI DISPLAY_TYPE;
typedef DisplayST7735_WxHx16_SPI DISPLAY_TYPE;

class Board_TTGO : public Board
{
    public:
        Board_TTGO();
        virtual ~Board_TTGO() {}

    public:
        virtual void init();
        virtual void run();

};

extern Board_TTGO board;
extern DISPLAY_TYPE   display;


#endif