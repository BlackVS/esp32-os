/*
    MIT License

    Copyright (c) 2016-2020, Alexey Dynda

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

///////////////////////////////////////////////////////////////////////////////
////// GENERIC FUNCTIONS APPLICABLE FOR ALL DISPLAY TYPES /////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "lcd_hal/io.h"
#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////////
//
//                            COMMON GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

template <class O>
void NanoDisplayOps<O>::putPixel(const NanoPoint &p)
{
    this->putPixel(p.x, p.y);
}

template <class O>
void NanoDisplayOps<O>::drawRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    this->drawHLine(x1, y1, x2);
    this->drawHLine(x1, y2, x2);
    this->drawVLine(x1, y1, y2);
    this->drawVLine(x2, y1, y2);
}

template <class O>
void NanoDisplayOps<O>::drawRect(const NanoRect &rect)
{
    this->drawRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

template <class O>
void NanoDisplayOps<O>::drawLine(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    lcduint_t  dx = x1 > x2 ? (x1 - x2): (x2 - x1);
    lcduint_t  dy = y1 > y2 ? (y1 - y2): (y2 - y1);
    lcduint_t  err = 0;
    if (dy > dx)
    {
        if (y1 > y2)
        {
            ssd1306_swap_data(x1, x2, lcdint_t);
            ssd1306_swap_data(y1, y2, lcdint_t);
        }
        for(; y1<=y2; y1++)
        {
            err += dx;
            if (err >= dy)
            {
                 err -= dy;
                 x1 < x2 ? x1++: x1--;
            }
            this->putPixel( x1, y1 );
        }
    }
    else
    {
        if (x1 > x2)
        {
            ssd1306_swap_data(x1, x2, lcdint_t);
            ssd1306_swap_data(y1, y2, lcdint_t);
        }
        for(; x1<=x2; x1++)
        {
            err += dy;
            if (err >= dx)
            {
                 err -= dx;
                 if (y1 < y2) y1++; else y1--;
            }
            this->putPixel( x1, y1 );
        }
    }
}

template <class O>
void NanoDisplayOps<O>::drawLine(const NanoRect &rect)
{
    this->drawLine(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

template <class O>
void NanoDisplayOps<O>::fillRect(const NanoRect &rect)
{
    this->fillRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

template <class O>
void NanoDisplayOps<O>::printFixedPgm(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
{
    this->m_fontStyle = style;
    this->m_cursorX = xpos;
    this->m_cursorY = y;
    for (;;)
    {
        char c = pgm_read_byte(ch);
        if (!c) break;
        this->write(c);
        ch++;
    }
}

template <class O>
void NanoDisplayOps<O>::write( const char *str )
{
    while (*str)
    {
        this->write(*str);
        str++;
    }
}

template <class O>
void NanoDisplayOps<O>::print( int number )
{
    char intStr[12];
    snprintf(intStr, sizeof(intStr), "%i", number );
    this->write( intStr );
}

#ifndef min
#define min(x,y) ((x)<(y)?(x):(y))
#endif

#ifndef max
#define max(x,y) ((x)>(y)?(x):(y))
#endif

template <class O>
static uint8_t getMaxScreenItems(NanoDisplayOps<O> &display, SAppMenu *menu)
{
    return (menu->height - 16) / display.getFont().getHeader().height;
}

template <class O>
static uint8_t calculateScrollPosition(NanoDisplayOps<O> &display, SAppMenu *menu, uint8_t selection)
{
    if ( selection < menu->scrollPosition )
    {
        return selection;
    }
    else if ( selection - menu->scrollPosition > getMaxScreenItems<O>(display, menu) - 1)
    {
        return selection - getMaxScreenItems<O>(display, menu) + 1;
    }
    return menu->scrollPosition;
}

template <class O>
static void drawMenuItem(NanoDisplayOps<O> &display, SAppMenu *menu, uint8_t index)
{
    if (index == menu->selection)
    {
        display.negativeMode();
    }
    else
    {
        display.positiveMode();
    }
    lcdint_t item_top = 8 + menu->top + (index - menu->scrollPosition)*display.getFont().getHeader().height;
    display.printFixed(menu->left + 8, item_top,
                       menu->items[index], STYLE_NORMAL );
    display.positiveMode();
}

template <class O>
static void drawMenuItemSmooth(NanoDisplayOps<O> &display, SAppMenu *menu, uint8_t index)
{
    if (index == menu->selection)
    {
        display.negativeMode();
    }
    else
    {
        display.positiveMode();
    }
    lcdint_t item_top = 8 + menu->top + (index - menu->scrollPosition)*display.getFont().getHeader().height;
    display.setColor( 0x0000 );
    display.fillRect( menu->left + 8 + display.getFont().getTextSize(menu->items[index]), item_top,
                      menu->width + menu->left - 9, item_top + display.getFont().getHeader().height - 1 );
    display.setColor( 0xFFFF );
    display.printFixed(menu->left + 8, item_top,
                       menu->items[index], STYLE_NORMAL );
    display.positiveMode();
}

template <class O>
void NanoDisplayOps<O>::createMenu(SAppMenu *menu, const char **items, uint8_t count, const NanoRect &rect)
{
    menu->items = items;
    menu->count = count;
    menu->selection = 0;
    menu->oldSelection = 0;
    menu->scrollPosition = 0;
    menu->top = rect.p1.y;
    menu->left = rect.p1.x;
    menu->width = rect.p2.x ? rect.width() : ( this->width() - menu->left );
    menu->height = rect.p2.y ? rect.height() : ( this->height() - menu->top );
}

template <class O>
void NanoDisplayOps<O>::showMenu( SAppMenu *menu )
{
    drawRect(4 + menu->left, 4 + menu->top, menu->width + menu->left - 5, menu->height + menu->top - 5);
    menu->scrollPosition = calculateScrollPosition<O>(*this, menu, menu->selection );
    for (uint8_t i = menu->scrollPosition; i < min(menu->count, (menu->scrollPosition + getMaxScreenItems<O>( *this, menu ))); i++)
    {
        drawMenuItem<O>(*this, menu, i);
    }
    menu->oldSelection = menu->selection;
}

template <class O>
void NanoDisplayOps<O>::showMenuSmooth( SAppMenu *menu )
{
    drawRect(4 + menu->left, 4 + menu->top, menu->width + menu->left - 5, menu->height + menu->top - 5);
    menu->scrollPosition = calculateScrollPosition<O>(*this, menu, menu->selection );
    for (uint8_t i = menu->scrollPosition; i < min(menu->count, (menu->scrollPosition + getMaxScreenItems<O>( *this, menu ))); i++)
    {
        drawMenuItemSmooth<O>(*this, menu, i);
    }
    menu->oldSelection = menu->selection;
}

template <class O>
void NanoDisplayOps<O>::updateMenu(SAppMenu *menu)
{
    if (menu->selection != menu->oldSelection)
    {
        uint8_t scrollPosition = calculateScrollPosition<O>( *this, menu, menu->selection );
        if ( scrollPosition != menu->scrollPosition )
        {
            this->clear();
            showMenu(menu);
        }
        else
        {
            drawMenuItem<O>( *this, menu, menu->oldSelection);
            drawMenuItem<O>( *this, menu, menu->selection);
            menu->oldSelection = menu->selection;
        }
    }
}

template <class O>
void NanoDisplayOps<O>::updateMenuSmooth(SAppMenu *menu)
{
    if (menu->selection != menu->oldSelection)
    {
        uint8_t scrollPosition = calculateScrollPosition<O>( *this, menu, menu->selection );
        if ( scrollPosition != menu->scrollPosition )
        {
            showMenuSmooth(menu);
        }
        else
        {
            drawMenuItemSmooth<O>( *this, menu, menu->oldSelection);
            drawMenuItemSmooth<O>( *this, menu, menu->selection);
            menu->oldSelection = menu->selection;
        }
    }
}

template <class O>
uint8_t NanoDisplayOps<O>::menuSelection(SAppMenu *menu)
{
    return menu->selection;
}

template <class O>
void NanoDisplayOps<O>::menuDown(SAppMenu *menu)
{
    if (menu->selection < menu->count - 1)
    {
        menu->selection++;
    }
    else
    {
        menu->selection = 0;
    }
}

template <class O>
void NanoDisplayOps<O>::menuUp(SAppMenu *menu)
{
    if (menu->selection > 0)
    {
        menu->selection--;
    }
    else
    {
        menu->selection = menu->count - 1;
    }
}

template <class O>
void NanoDisplayOps<O>::drawCanvas(lcdint_t x, lcdint_t y, NanoCanvasOps<1> &canvas)
{
    this->drawBuffer1Fast( x, y, canvas.width(), canvas.height(), canvas.getData() );
}

template <class O>
void NanoDisplayOps<O>::drawCanvas(lcdint_t x, lcdint_t y, NanoCanvasOps<4> &canvas)
{
    this->drawBuffer4( x, y, canvas.width(), canvas.height(), canvas.getData() );
}

template <class O>
void NanoDisplayOps<O>::drawCanvas(lcdint_t x, lcdint_t y, NanoCanvasOps<8> &canvas)
{
    this->drawBuffer8( x, y, canvas.width(), canvas.height(), canvas.getData() );
}

template <class O>
void NanoDisplayOps<O>::drawCanvas(lcdint_t x, lcdint_t y, NanoCanvasOps<16> &canvas)
{
    this->drawBuffer16( x, y, canvas.width(), canvas.height(), canvas.getData() );
}

template <class O>
void NanoDisplayOps<O>::drawProgressBar(int8_t progress)
{
    lcduint_t height = 8;
    lcduint_t width = 8;
    char str[5] = "100%";
    if ( progress < 100 )
    {
        str[0] = ' ';
        str[1] = progress / 10 + '0';
        str[2] = progress % 10 + '0';
        str[3] = '%';
    }
    if ( this->m_font != nullptr )
    {
        width = this->getFont().getTextSize( str, &height );
    }
    lcdint_t middle = this->height() / 2;
    lcdint_t progress_pos = 8 + (int16_t)(this->width() - 16) * progress / 100;
    uint16_t color = this->m_color;
    this->m_color = 0x0000;
    this->fillRect( progress_pos, middle, this->width() - 8, middle + height );
    this->m_color = color;
    this->drawRect( progress_pos, middle, this->width() - 8, middle + height );
    this->fillRect( 8, middle, progress_pos, middle + height );
    if ( this->m_font != nullptr )
    {
        this->printFixed( this->width() / 2 - width / 2, middle - height, str );
    }
}

template <class O>
void NanoDisplayOps<O>::drawWindow(lcdint_t x, lcdint_t y,
                                     lcduint_t width, lcduint_t height,
                                     const char *caption, bool blank)
{
    if ( width == 0 ) { width = this->width() - 8; x = 4; }
    if ( height == 0 ) { height = this->height() - 4; y = 0; }
    if ( blank )
    {
        uint16_t color = this->m_color;
        this->m_color = 0x0000;
        this->fillRect( x, y, x + width - 1, y + height - 1 );
        this->m_color = color;
    }
    if ( caption )
    {
        y += this->getFont().getHeader().height / 2;
        height -= this->getFont().getHeader().height / 2;
    }
    this->drawRect( x, y, x + width - 1, y + height - 1 );
    if ( caption )
    {
        lcduint_t theight;
        lcduint_t twidth = this->getFont().getTextSize( caption, &theight );
        this->printFixed( x + (width - twidth)/2, y - theight/2, caption );
    }
}

