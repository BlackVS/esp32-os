/*
    MIT License

    Copyright (c) 2018-2019, Alexey Dynda

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

//#include "platform.h"
#include "display.h"

//extern uint16_t ssd1306_color;
extern uint8_t s_ssd1306_invertByte;
//extern unsigned int ssd1306_cursorX;
//extern unsigned int ssd1306_cursorY;


/////////////////////////////////////////////////////////////////////////////////
//
//                             16-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////



template<>
void NanoDisplayOpsX<16>::putPixel(int x, int y)
{
    m_intf.startBlock(x, y, 0);
    m_intf.send( this->m_color >> 8 );
    m_intf.send( this->m_color & 0xFF );
    m_intf.endBlock();
}



template<>
void NanoDisplayOpsX<16>::drawHLine(int x1, int y1, int x2)
{
    m_intf.startBlock(x1, y1, 0);
    while (x1 < x2)
    {
        m_intf.send( this->m_color >> 8 );
        m_intf.send( this->m_color & 0xFF );
        x1++;
    }
    m_intf.endBlock();
}



template<>
void NanoDisplayOpsX<16>::drawVLine(int x1, int y1, int y2)
{
    m_intf.startBlock(x1, y1, 1);
    while (y1<=y2)
    {
        m_intf.send( this->m_color >> 8 );
        m_intf.send( this->m_color & 0xFF );
        y1++;
    }
    m_intf.endBlock();
}



template<>
void NanoDisplayOpsX<16>::fillRect(int x1, int y1, int x2, int y2)
{
    if (y1 > y2)
    {
        ssd1306_swap_data(y1, y2, int);
    }
    if (x1 > x2)
    {
        ssd1306_swap_data(x1, x2, int);
    }
    m_intf.startBlock(x1, y1, x2 - x1 + 1);
    uint16_t count = (x2 - x1 + 1) * (y2 - y1 + 1);
    while (count--)
    {
        m_intf.send( this->m_color >> 8 );
        m_intf.send( this->m_color & 0xFF );
    }
    m_intf.endBlock();
}



template<>
void NanoDisplayOpsX<16>::fill(uint16_t color)
{
    m_intf.startBlock(0, 0, 0);
    uint32_t count = (uint32_t)this->m_w * (uint32_t)this->m_h;
    while (count--)
    {
        m_intf.send( color >> 8 );
        m_intf.send( color & 0xFF );
    }
    m_intf.endBlock();
}



template<>
void NanoDisplayOpsX<16>::drawXBitmap(int x, int y, unsigned int w, unsigned int h, const uint8_t *bitmap)
{
    // TODO:
}



template<>
void NanoDisplayOpsX<16>::drawBitmap1(int xpos, int ypos, unsigned int w, unsigned int h, const uint8_t *bitmap)
{
    uint8_t bit = 1;
    uint16_t blackColor = s_ssd1306_invertByte ? this->m_color : 0x0000;
    uint16_t color = s_ssd1306_invertByte ? 0x0000 : this->m_color;
    m_intf.startBlock(xpos, ypos, w);
    while (h--)
    {
        unsigned int wx = w;
        while ( wx-- )
        {
            uint8_t data = pgm_read_byte( bitmap );
            if ( data & bit )
            {
                m_intf.send( color >> 8 );
                m_intf.send( color & 0xFF );
            }
            else
            {
                m_intf.send( blackColor >> 8 );
                m_intf.send( blackColor & 0xFF );
            }
            bitmap++;
        }
        bit <<= 1;
        if ( bit == 0 )
        {
            bit = 1;
        }
        else
        {
            bitmap -= w;
        }
    }
    m_intf.endBlock();
}



template<>
void NanoDisplayOpsX<16>::drawBitmap4(int x, int y, unsigned int w, unsigned int h, const uint8_t *bitmap)
{
    // NOT IMPLEMENTED
}



template<>
void NanoDisplayOpsX<16>::drawBitmap8(int x, int y, unsigned int w, unsigned int h, const uint8_t *bitmap)
{
    m_intf.startBlock(x, y, w);
    uint32_t count = (w) * (h);
    while (count--)
    {
        uint16_t color = RGB8_TO_RGB16(pgm_read_byte(bitmap));
        m_intf.send( color >> 8 );
        m_intf.send( color & 0xFF );
        bitmap++;
    }
    m_intf.endBlock();
}



template<>
void NanoDisplayOpsX<16>::drawBitmap16(int x, int y, unsigned int w, unsigned int h, const uint8_t *bitmap)
{
    m_intf.startBlock(x, y, w);
    uint32_t count = (w) * (h);
    while (count--)
    {
        m_intf.send( pgm_read_byte( &bitmap[0] ) );
        m_intf.send( pgm_read_byte( &bitmap[1] ) );
        bitmap += 2;
    }
    m_intf.endBlock();
}



template<>
void NanoDisplayOpsX<16>::drawBuffer1(int xpos, int ypos, unsigned int w, unsigned int h, const uint8_t *buffer)
{
    uint8_t bit = 1;
    uint16_t blackColor = s_ssd1306_invertByte ? this->m_color : 0x0000;
    uint16_t color = s_ssd1306_invertByte ? 0x0000 : this->m_color;
    m_intf.startBlock(xpos, ypos, w);
    while (h--)
    {
        unsigned int wx = w;
        while ( wx-- )
        {
            uint8_t data = *buffer;
            if ( data & bit )
            {
                m_intf.send( color >> 8 );
                m_intf.send( color & 0xFF );
            }
            else
            {
                m_intf.send( blackColor >> 8 );
                m_intf.send( blackColor & 0xFF );
            }
            buffer++;
        }
        bit <<= 1;
        if ( bit == 0 )
        {
            bit = 1;
        }
        else
        {
            buffer -= w;
        }
    }
    m_intf.endBlock();
}



template<>
void NanoDisplayOpsX<16>::drawBuffer1Fast(int x, int y, unsigned int w, unsigned int h, const uint8_t *buf)
{
    this->drawBuffer1( x, y, w, h, buf );
}



template<>
void NanoDisplayOpsX<16>::drawBuffer4(int x, int y, unsigned int w, unsigned int h, const uint8_t *buffer)
{
    // NOT IMPLEMENTED
}



template<>
void NanoDisplayOpsX<16>::drawBuffer8(int x, int y, unsigned int w, unsigned int h, const uint8_t *buffer)
{
    m_intf.startBlock(x, y, w);
    uint32_t count = (w) * (h);
    while (count--)
    {
        uint16_t color = RGB8_TO_RGB16((*buffer));
        m_intf.send( color >> 8 );
        m_intf.send( color & 0xFF );
        buffer++;
    }
    m_intf.endBlock();
}



template<>
void NanoDisplayOpsX<16>::drawBuffer16(int x, int y, unsigned int w, unsigned int h, const uint8_t *buffer)
{
    m_intf.startBlock(x, y, w);
    uint32_t count = (w) * (h);
    while (count--)
    {
        m_intf.send( buffer[0] );
        m_intf.send( buffer[1] );
        buffer += 2;
    }
    m_intf.endBlock();
}



template<>
uint8_t NanoDisplayOpsX<16>::printChar(uint8_t c)
{
    uint16_t unicode = this->m_font->unicode16FromUtf8(c);
    if (unicode == SSD1306_MORE_CHARS_REQUIRED) return 0;
    SCharInfo char_info;
    this->m_font->getCharBitmap(unicode, &char_info);
    uint8_t mode = this->m_textMode;
    for (uint8_t i = 0; i<(this->m_fontStyle == STYLE_BOLD ? 2: 1); i++)
    {
        this->drawBitmap1(this->m_cursorX + i,
                    this->m_cursorY,
                    char_info.width,
                    char_info.height,
                    char_info.glyph );
        this->m_textMode |= CANVAS_MODE_TRANSPARENT;
    }
    this->m_textMode = mode;
    this->m_cursorX += (int)(char_info.width + char_info.spacing);
    if ( ( (this->m_textMode & CANVAS_TEXT_WRAP_LOCAL) &&
           (this->m_cursorX > ((int)this->m_w - (int)this->m_font->getHeader().width) ) )
       || ( (this->m_textMode & CANVAS_TEXT_WRAP) &&
           (this->m_cursorX > ((int)this->m_w - (int)this->m_font->getHeader().width)) ) )
    {
        this->m_cursorY += (int)this->m_font->getHeader().height;
        this->m_cursorX = 0;
        if ( (this->m_textMode & CANVAS_TEXT_WRAP_LOCAL) &&
             (this->m_cursorY > ((int)this->m_h - (int)this->m_font->getHeader().height)) )
        {
            this->m_cursorY = 0;
        }
    }
    return 1;
}



template<>
size_t NanoDisplayOpsX<16>::write(uint8_t c)
{
    if (c == '\n')
    {
        this->m_cursorY += (int)this->m_font->getHeader().height;
        this->m_cursorX = 0;
    }
    else if (c == '\r')
    {
        // skip non-printed char
    }
    else
    {
        return printChar( c );
    }
    return 1;
}



template<>
void NanoDisplayOpsX<16>::printFixed(int xpos, int y, const char *ch, EFontStyle style)
{
    // TODO: fontstyle not supported
    // m_fontStyle = style;
    this->m_cursorX = xpos;
    this->m_cursorY = y;
    while (*ch)
    {
        this->write(*ch);
        ch++;
    }
}


