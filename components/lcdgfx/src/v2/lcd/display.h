/*
    MIT License

    Copyright (c) 2018-2020, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once

#include "canvas/point.h"
#include "canvas/rect.h"
#include "canvas/canvas.h"
// #include "canvas/font.h"
#include "platform.h"
// #include "nano_gfx_types.h"
// #include "display_base.h"

#include "display_Xbits.h"


/**
 * NanoCanvasOps provides operations for drawing in memory buffer.
 * Depending on BPP argument, this class can work with 1,8,16-bit canvas areas.
 */
template <uint8_t BPP>
class NanoDisplayOps: public NanoDisplayOpsX<BPP>
{
public:
    using NanoDisplayOpsX<BPP>::NanoDisplayOpsX;

    using NanoDisplayOpsX<BPP>::putPixel;

    /**
     * Draws pixel on specified position
     * @param p - NanoPoint
     * @note color can be set via setColor()
     */
    void putPixel(const NanoPoint &p);

    /**
     * Draws line
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void drawLine(int x1, int y1, int x2, int y2);

    /**
     * Draws line
     * @param rect - structure, describing rectangle area
     * @note color can be set via setColor()
     */
    void drawLine(const NanoRect &rect);

    /**
     * Draws rectangle
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void drawRect(int x1, int y1, int x2, int y2);
    // {
    //     this->drawHLine(x1, y1, x2);
    //     this->drawHLine(x1, y2, x2);
    //     this->drawVLine(x1, y1, y2);
    //     this->drawVLine(x2, y1, y2);
    // }
    /**
     * Draws rectangle
     * @param rect - structure, describing rectangle area
     * @note color can be set via setColor()
     */
    void drawRect(const NanoRect &rect);

    using NanoDisplayOpsX<BPP>::fillRect;

    /**
     * Fills rectangle area
     * @param rect - structure, describing rectangle area
     * @note color can be set via setColor()
     */
    void fillRect(const NanoRect &rect);

    /**
     * Draws 1-bit canvas on lcd display
     *
     * @param x x position in pixels
     * @param y y position in pixels
     * @param canvas 1-bit canvas to draw on the screen.
     */
    void drawCanvas(int x, int y, NanoCanvasOps<1> &canvas)
    {
        this->drawBuffer1Fast( x, y, canvas.width(), canvas.height(), canvas.getData() );
    }

    /**
     * Draws 4-bit canvas on lcd display
     *
     * @param x x position in pixels
     * @param y y position in pixels
     * @param canvas 4-bit canvas to draw on the screen.
     */
    void drawCanvas(int x, int y, NanoCanvasOps<4> &canvas)
    {
        this->drawBuffer4( x, y, canvas.width(), canvas.height(), canvas.getData() );
    }

    /**
     * Draws 8-bit canvas on lcd display
     *
     * @param x x position in pixels
     * @param y y position in pixels
     * @param canvas 8-bit canvas to draw on the screen.
     */
    void drawCanvas(int x, int y, NanoCanvasOps<8> &canvas)
    {
        this->drawBuffer8( x, y, canvas.width(), canvas.height(), canvas.getData() );
    }

    /**
     * Draws 16-bit canvas on lcd display
     *
     * @param x x position in pixels
     * @param y y position in pixels
     * @param canvas 16-bit canvas to draw on the screen.
     */
    void drawCanvas(int x, int y, NanoCanvasOps<16> &canvas)
    {
        this->drawBuffer16( x, y, canvas.width(), canvas.height(), canvas.getData() );
    }

    /**
     * Print text at specified position to canvas
     *
     * @param xpos  position in pixels
     * @param y     position in pixels
     * @param ch    pointer to NULL-terminated string, located in flash
     * @param style specific font style to use
     *
     * @note Supports only STYLE_NORMAL and STYLE_BOLD
     */
    void printFixedPgm(int xpos, int y, const char *ch, EFontStyle style = STYLE_NORMAL);

    using NanoDisplayOpsX<BPP>::write;

    /**
     * Prints text at current cursor position.
     * To specify cursor position using setTextCursor() method.
     *
     * @param str text to print (null-terminated string)
     */
    void write(const char *str);

    /**
     * Prints number at current cursor position
     * To specify cursor position using setTextCursor() method.
     *
     * @param number integer number to print
     */
    void print(int number);

    /**
     * Creates menu object with the provided list of menu items.
     * List of menu items (strings) must exist all until menu object is no longer needed.
     * Selection is set to the first item by default.
     *
     * @param menu pointer to SAppMenu structure
     * @param items array of null-termintated strings (located in SRAM)
     * @param count count of menu items in the array
     * @param rect screen area to use for menu
     */
    void createMenu(SAppMenu *menu, const char **items, uint8_t count, const NanoRect &rect = {});

    /**
     * Shows menu items on the display. If menu items cannot fit the display,
     * the function provides scrolling.
     *
     * @param menu pointer to SAppMenu structure
     */
    void showMenu( SAppMenu *menu);

    /**
     * Shows menu items on the display. If menu items cannot fit the display,
     * the function provides scrolling. Unlike showMenu() this implementation
     * uses more graph functions and requires more flash, but result looks better.
     *
     * @param menu pointer to SAppMenu structure
     */
    void showMenuSmooth( SAppMenu *menu );

    /**
     * Updates menu items on the display. That is if selection is changed,
     * the function will update only those areas, affected by the change.
     *
     * @param menu Pointer to SAppMenu structure
     */
    void updateMenu(SAppMenu *menu);

    /**
     * Updates menu items on the display. That is if selection is changed,
     * the function will update only those areas, affected by the change.
     * Unlike showMenu() this implementation uses more graph functions
     * and requires more flash, but result looks better.
     *
     * @param menu Pointer to SAppMenu structure
     */
    void updateMenuSmooth(SAppMenu *menu);

    /**
     * Returns currently selected menu item.
     * First item has zero-index.
     *
     * @param menu pointer to SAppMenu structure
     *
     */
    uint8_t menuSelection(SAppMenu *menu);

    /**
     * Moves selection pointer down by 1 item. If there are no items below,
     * it will set selection pointer to the first item.
     * Use updateMenu() to refresh menu state on the display.
     *
     * @param menu pointer to SAppMenu structure
     */
    void menuDown(SAppMenu *menu);

    /**
     * Moves selection pointer up by 1 item. If selected item is the first one,
     * then selection pointer will set to the last item in menu list.
     * Use updateMenu() to refresh menu state on the display.
     *
     * @param menu pointer to SAppMenu structure
     */
    void menuUp(SAppMenu *menu);

    /**
     * Displays progress bar in the middle of the display.
     *
     * @param progress progress in range 0 - 100.
     */
    void drawProgressBar(int8_t progress);

    /**
     * Displays window at specified position and of specified size
     *
     * @param x x position in pixels from top-lef corner
     * @param y y position in pixels from top-lef corner
     * @param width width of window to draw. Can be 0.
     * @param height height of windows to draw. Can be 0.
     * @param caption text to display as caption
     * @param blank true if content inside window should be blanked
     */
    void drawWindow(int x, int y,
                    unsigned int width, unsigned int height,
                    const char *caption, bool blank);

protected:
    /**
     * Initializes interface and display
     */
    virtual void begin() = 0;

    /**
     * closes interface to lcd display
     */
    virtual void end() = 0;
};


#include "display.inl"

