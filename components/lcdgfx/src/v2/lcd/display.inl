#include "esp32_tools.h"
#include <cstdio>

template <uint8_t BPP>
void NanoDisplayOps<BPP>::putPixel(const NanoPoint &p)
{
    this->putPixel(p.x, p.y);
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::drawRect(int x1, int y1, int x2, int y2)
{
    this->drawHLine(x1, y1, x2);
    this->drawHLine(x1, y2, x2);
    this->drawVLine(x1, y1, y2);
    this->drawVLine(x2, y1, y2);
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::drawRect(const NanoRect &rect)
{
    this->drawRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::drawLine(int x1, int y1, int x2, int y2)
{
    unsigned int  dx = x1 > x2 ? (x1 - x2): (x2 - x1);
    unsigned int  dy = y1 > y2 ? (y1 - y2): (y2 - y1);
    unsigned int  err = 0;
    if (dy > dx)
    {
        if (y1 > y2)
        {
            SWAP(x1, x2);
            SWAP(y1, y2);
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
            SWAP(x1, x2);
            SWAP(y1, y2);
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

template <uint8_t BPP>
void NanoDisplayOps<BPP>::drawLine(const NanoRect &rect)
{
    this->drawLine(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::fillRect(const NanoRect &rect)
{
    this->fillRect(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::printFixedPgm(int xpos, int y, const char *ch, EFontStyle style)
{
    this->m_fontStyle = style;
    this->m_cursorX = xpos;
    this->m_cursorY = y;
    for (;;)
    {
        char c = ch[0];
        if (!c) break;
        this->write(c);
        ch++;
    }
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::write( const char *str )
{
    while (*str)
    {
        this->write(*str);
        str++;
    }
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::print( int number )
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

template <uint8_t BPP>
static uint8_t getMaxScreenItems(NanoDisplayOps<BPP> &display, SAppMenu *menu)
{
    return (menu->height - 16) / display.getFont().getHeader().height;
}

template <uint8_t BPP>
static uint8_t calculateScrollPosition(NanoDisplayOps<BPP> &display, SAppMenu *menu, uint8_t selection)
{
    if ( selection < menu->scrollPosition )
    {
        return selection;
    }
    else if ( selection - menu->scrollPosition > getMaxScreenItems<BPP>(display, menu) - 1)
    {
        return selection - getMaxScreenItems<BPP>(display, menu) + 1;
    }
    return menu->scrollPosition;
}

template <uint8_t BPP>
static void drawMenuItem(NanoDisplayOps<BPP> &display, SAppMenu *menu, uint8_t index)
{
    if (index == menu->selection)
    {
        display.negativeMode();
    }
    else
    {
        display.positiveMode();
    }
    int item_top = 8 + menu->top + (index - menu->scrollPosition)*display.getFont().getHeader().height;
    display.printFixed(menu->left + 8, item_top,
                       menu->items[index], STYLE_NORMAL );
    display.positiveMode();
}

template <uint8_t BPP>
static void drawMenuItemSmooth(NanoDisplayOps<BPP> &display, SAppMenu *menu, uint8_t index)
{
    if (index == menu->selection)
    {
        display.negativeMode();
    }
    else
    {
        display.positiveMode();
    }
    int item_top = 8 + menu->top + (index - menu->scrollPosition)*display.getFont().getHeader().height;
    display.setColor( 0x0000 );
    display.fillRect( menu->left + 8 + display.getFont().getTextSize(menu->items[index]), item_top,
                      menu->width + menu->left - 9, item_top + display.getFont().getHeader().height - 1 );
    display.setColor( 0xFFFF );
    display.printFixed(menu->left + 8, item_top,
                       menu->items[index], STYLE_NORMAL );
    display.positiveMode();
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::createMenu(SAppMenu *menu, const char **items, uint8_t count, const NanoRect &rect)
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

template <uint8_t BPP>
void NanoDisplayOps<BPP>::showMenu( SAppMenu *menu )
{
    drawRect(4 + menu->left, 4 + menu->top, menu->width + menu->left - 5, menu->height + menu->top - 5);
    menu->scrollPosition = calculateScrollPosition<BPP>(*this, menu, menu->selection );
    for (uint8_t i = menu->scrollPosition; i < min(menu->count, (menu->scrollPosition + getMaxScreenItems<BPP>( *this, menu ))); i++)
    {
        drawMenuItem<BPP>(*this, menu, i);
    }
    menu->oldSelection = menu->selection;
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::showMenuSmooth( SAppMenu *menu )
{
    drawRect(4 + menu->left, 4 + menu->top, menu->width + menu->left - 5, menu->height + menu->top - 5);
    menu->scrollPosition = calculateScrollPosition<BPP>(*this, menu, menu->selection );
    for (uint8_t i = menu->scrollPosition; i < min(menu->count, (menu->scrollPosition + getMaxScreenItems<BPP>( *this, menu ))); i++)
    {
        drawMenuItemSmooth<BPP>(*this, menu, i);
    }
    menu->oldSelection = menu->selection;
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::updateMenu(SAppMenu *menu)
{
    if (menu->selection != menu->oldSelection)
    {
        uint8_t scrollPosition = calculateScrollPosition<BPP>( *this, menu, menu->selection );
        if ( scrollPosition != menu->scrollPosition )
        {
            this->clear();
            showMenu(menu);
        }
        else
        {
            drawMenuItem<BPP>( *this, menu, menu->oldSelection);
            drawMenuItem<BPP>( *this, menu, menu->selection);
            menu->oldSelection = menu->selection;
        }
    }
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::updateMenuSmooth(SAppMenu *menu)
{
    if (menu->selection != menu->oldSelection)
    {
        uint8_t scrollPosition = calculateScrollPosition<BPP>( *this, menu, menu->selection );
        if ( scrollPosition != menu->scrollPosition )
        {
            showMenuSmooth(menu);
        }
        else
        {
            drawMenuItemSmooth<BPP>( *this, menu, menu->oldSelection);
            drawMenuItemSmooth<BPP>( *this, menu, menu->selection);
            menu->oldSelection = menu->selection;
        }
    }
}

template <uint8_t BPP>
uint8_t NanoDisplayOps<BPP>::menuSelection(SAppMenu *menu)
{
    return menu->selection;
}

template <uint8_t BPP>
void NanoDisplayOps<BPP>::menuDown(SAppMenu *menu)
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

template <uint8_t BPP>
void NanoDisplayOps<BPP>::menuUp(SAppMenu *menu)
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

// template <uint8_t BPP>
// void NanoDisplayOps<BPP>::drawCanvas(int x, int y, NanoCanvasOps<1> &canvas)
// {
//     this->drawBuffer1Fast( x, y, canvas.width(), canvas.height(), canvas.getData() );
// }

// template <uint8_t BPP>
// void NanoDisplayOps<BPP>::drawCanvas(int x, int y, NanoCanvasOps<4> &canvas)
// {
//     this->drawBuffer4( x, y, canvas.width(), canvas.height(), canvas.getData() );
// }

// template <uint8_t BPP>
// void NanoDisplayOps<BPP>::drawCanvas(int x, int y, NanoCanvasOps<8> &canvas)
// {
//     this->drawBuffer8( x, y, canvas.width(), canvas.height(), canvas.getData() );
// }

// template <uint8_t BPP>
// void NanoDisplayOps<BPP>::drawCanvas(int x, int y, NanoCanvasOps<16> &canvas)
// {
//     this->drawBuffer16( x, y, canvas.width(), canvas.height(), canvas.getData() );
// }

template <uint8_t BPP>
void NanoDisplayOps<BPP>::drawProgressBar(int8_t progress)
{
    unsigned int height = 8;
    unsigned int width = 8;
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
    int middle = this->height() / 2;
    int progress_pos = 8 + (int16_t)(this->width() - 16) * progress / 100;
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

template <uint8_t BPP>
void NanoDisplayOps<BPP>::drawWindow(int x, int y,
                                     unsigned int width, unsigned int height,
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
        unsigned int theight;
        unsigned int twidth = this->getFont().getTextSize( caption, &theight );
        this->printFixed( x + (width - twidth)/2, y - theight/2, caption );
    }
}

