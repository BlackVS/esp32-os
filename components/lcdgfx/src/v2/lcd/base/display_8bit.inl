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

#include "lcd_hal/io.h"

extern uint8_t s_ssd1306_invertByte;

#if 0
void    ssd1306_setRgbColor(uint8_t r, uint8_t g, uint8_t b)
{
    ssd1306_color = RGB_COLOR8(r,g,b);
}

void    ssd1306_setRgbColor8(uint8_t r, uint8_t g, uint8_t b)
{
    ssd1306_color = RGB_COLOR8(r,g,b);
}

static void ssd1306_drawBufferPitch8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, lcduint_t pitch, const uint8_t *data)
{
    ssd1306_lcd.set_block(x, y, w);
    while (h--)
    {
        lcduint_t line = w;
        while (line--)
        {
            ssd1306_lcd.send_pixels8( *data );
            data++;
        }
        data += pitch - w;
    }
    ssd1306_intf.stop();
}

void ssd1306_drawBufferFast8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *data)
{
    ssd1306_drawBufferPitch8( x, y, w, h, w, data );
}

void ssd1306_drawBufferEx8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, lcduint_t pitch, const uint8_t *data)
{
    ssd1306_drawBufferPitch8( x, y, w, h, pitch, data );
}

void ssd1306_putColorPixel8(lcdint_t x, lcdint_t y, uint8_t color)
{
    ssd1306_lcd.set_block(x, y, 0);
    ssd1306_lcd.send_pixels8( color );
    ssd1306_intf.stop();
}

#endif

/////////////////////////////////////////////////////////////////////////////////
//
//                            COMMON GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

//template class NanoDisplayOps8;

//
//void NanoDisplayOps8::printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
//{
//    m_cursorX = xpos;
//    m_cursorY = y;
//    m_fontStyle = style;
//    print( ch );
//}

/////////////////////////////////////////////////////////////////////////////////
//
//                             8-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////


void NanoDisplayOps8::putPixel(lcdint_t x, lcdint_t y)
{
    m_intf.startBlock(x, y, 0);
    m_intf.send( this->m_color );
    m_intf.endBlock();
}


void NanoDisplayOps8::drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2)
{
    m_intf.startBlock(x1, y1, 0);
    while (x1 < x2)
    {
        m_intf.send( this->m_color );
        x1++;
    }
    m_intf.endBlock();
}


void NanoDisplayOps8::drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2)
{
    m_intf.startBlock(x1, y1, 1);
    while (y1<=y2)
    {
        m_intf.send( this->m_color );
        y1++;
    }
    m_intf.endBlock();
}


void NanoDisplayOps8::fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)
{
    if (y1 > y2)
    {
        ssd1306_swap_data(y1, y2, lcdint_t);
    }
    if (x1 > x2)
    {
        ssd1306_swap_data(x1, x2, lcdint_t);
    }
    m_intf.startBlock(x1, y1, x2 - x1 + 1);
    uint32_t count = (x2 - x1 + 1) * (y2 - y1 + 1);
    while (count--)
    {
        m_intf.send( this->m_color );
    }
    m_intf.endBlock();
}


void NanoDisplayOps8::fill(uint16_t color)
{
    m_intf.startBlock(0, 0, 0);
    uint32_t count = (uint32_t)this->m_w * (uint32_t)this->m_h;
    while (count--)
    {
        m_intf.send( color );
    }
    m_intf.endBlock();
}


void NanoDisplayOps8::clear()
{
    fill( 0x00 );
}


void NanoDisplayOps8::drawXBitmap(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    // TODO:
}


void NanoDisplayOps8::drawBitmap1(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    uint8_t bit = 1;
    uint8_t blackColor = s_ssd1306_invertByte ? this->m_color : 0x00;
    uint8_t color = s_ssd1306_invertByte ? 0x00 : this->m_color;
    m_intf.startBlock(xpos, ypos, w);
    while (h--)
    {
        lcduint_t wx = w;
        while ( wx-- )
        {
            uint8_t data = pgm_read_byte( bitmap );
            if ( data & bit )
            {
                m_intf.send( color );
            }
            else
            {
                m_intf.send( blackColor );
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


void NanoDisplayOps8::drawBitmap4(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    // NOT IMPLEMENTED
}


void NanoDisplayOps8::drawBitmap8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    m_intf.startBlock(x, y, w);
    uint32_t count = (w) * (h);
    while (count--)
    {
        m_intf.send( pgm_read_byte( bitmap ) );
        bitmap++;
    }
    m_intf.endBlock();
}


void NanoDisplayOps8::drawBitmap16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap)
{
    // NOT IMPLEMENTED
}


void NanoDisplayOps8::drawBuffer1(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    uint8_t bit = 1;
    uint8_t blackColor = s_ssd1306_invertByte ? this->m_color : 0x00;
    uint8_t color = s_ssd1306_invertByte ? 0x00 : this->m_color;
    m_intf.startBlock(xpos, ypos, w);
    while (h--)
    {
        lcduint_t wx = w;
        while ( wx-- )
        {
            uint8_t data = *buffer;
            if ( data & bit )
            {
                m_intf.send( color );
            }
            else
            {
                m_intf.send( blackColor );
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


void NanoDisplayOps8::drawBuffer1Fast(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buf)
{
    this->drawBuffer1( x, y, w, h, buf );
}


void NanoDisplayOps8::drawBuffer4(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    // NOT IMPLEMENTED
}


void NanoDisplayOps8::drawBuffer8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    m_intf.startBlock(x, y, w);
    uint32_t count = (w) * (h);
    while (count--)
    {
        m_intf.send( *buffer );
        buffer++;
    }
    m_intf.endBlock();
}


void NanoDisplayOps8::drawBuffer16(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer)
{
    // NOT IMPLEMENTED
}


uint8_t NanoDisplayOps8::printChar(uint8_t c)
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
    this->m_cursorX += (lcdint_t)(char_info.width + char_info.spacing);
    if ( ( (this->m_textMode & CANVAS_TEXT_WRAP_LOCAL) &&
           (this->m_cursorX > ((lcdint_t)this->m_w - (lcdint_t)this->m_font->getHeader().width) ) )
       || ( (this->m_textMode & CANVAS_TEXT_WRAP) &&
           (this->m_cursorX > ((lcdint_t)this->m_w - (lcdint_t)this->m_font->getHeader().width)) ) )
    {
        this->m_cursorY += (lcdint_t)this->m_font->getHeader().height;
        this->m_cursorX = 0;
        if ( (this->m_textMode & CANVAS_TEXT_WRAP_LOCAL) &&
             (this->m_cursorY > ((lcdint_t)this->m_h - (lcdint_t)this->m_font->getHeader().height)) )
        {
            this->m_cursorY = 0;
        }
    }
    return 1;
}


size_t NanoDisplayOps8::write(uint8_t c)
{
    if (c == '\n')
    {
        this->m_cursorY += (lcdint_t)this->m_font->getHeader().height;
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


void NanoDisplayOps8::printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style)
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

