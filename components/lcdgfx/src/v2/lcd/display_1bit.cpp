/////////////////////////////////////////////////////////////////////////////////
//
//                             1-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

#include "display_Xbits.h"

template<>
void NanoDisplayOpsX<1>::printFixed(int xpos, int y, const char *ch, EFontStyle style)
{
    uint8_t i, j=0;
    uint8_t text_index = 0;
    uint8_t page_offset = 0;
    uint8_t x = xpos;
    y >>= 3;
    m_intf.startBlock(xpos, y, this->m_w - xpos);
    for(;;)
    {
        uint8_t ldata;
        if ( (x > this->m_w - this->m_font->getHeader().width) || (ch[j] == '\0') )
        {
            x = xpos;
            y++;
            if (y >= (int)( this->m_h >> 3))
            {
                break;
            }
            page_offset++;
            if (page_offset == this->m_font->getPages())
            {
                text_index = j;
                page_offset = 0;
                if (ch[j] == '\0')
                {
                    break;
                }
            }
            else
            {
                j = text_index;
            }
            m_intf.endBlock();
            m_intf.startBlock(xpos, y, this->m_w - xpos);
        }
        uint16_t unicode;
        do
        {
            unicode = this->m_font->unicode16FromUtf8(ch[j]);
            j++;
        } while ( unicode == SSD1306_MORE_CHARS_REQUIRED );
        SCharInfo char_info;
        this->m_font->getCharBitmap(unicode, &char_info);
        ldata = 0;
        x += char_info.width + char_info.spacing;
        if (char_info.height > page_offset * 8)
        {
            char_info.glyph += page_offset * char_info.width;
            for( i = char_info.width; i>0; i--)
            {
                uint8_t data;
                if ( style == STYLE_NORMAL )
                {
                    data = char_info.glyph[0];
                }
                else if ( style == STYLE_BOLD )
                {
                    uint8_t temp = char_info.glyph[0];
                    data = temp | ldata;
                    ldata = temp;
                }
                else
                {
                    uint8_t temp = char_info.glyph[1];
                    data = (temp & 0xF0) | ldata;
                    ldata = (temp & 0x0F);
                }
                m_intf.send(data^s_ssd1306_invertByte);
                char_info.glyph++;
            }
        }
        else
        {
            char_info.spacing += char_info.width;
        }
        for (i = 0; i < char_info.spacing; i++)
            m_intf.send(s_ssd1306_invertByte);
    }
    m_intf.endBlock();
}

template<>
void NanoDisplayOpsX<1>::drawBitmap1(int x, int y, unsigned int w, unsigned int h, const uint8_t *bitmap)
{
    unsigned int origin_width = w;
    uint8_t offset = y & 0x07;
    uint8_t complexFlag = 0;
    uint8_t mainFlag = 1;
    uint8_t max_pages;
    uint8_t pages;
    unsigned int i, j;
    if (y + (int)h <= 0) return;
    if (y >= (int)this->m_h) return;
    if (x + (int)w <= 0) return;
    if (x >= (int)this->m_w)  return;
    if (y < 0)
    {
         bitmap += ((unsigned int)((-y) + 7) >> 3) * w;
         h += y;
         y = 0;
         complexFlag = 1;
    }
    if (x < 0)
    {
         bitmap += -x;
         w += x;
         x = 0;
    }
    max_pages = (unsigned int)(h + 15 - offset) >> 3;
    if ((unsigned int)((unsigned int)y + h) > (unsigned int)this->m_h)
    {
         h = (unsigned int)(this->m_h - (unsigned int)y);
    }
    if ((unsigned int)((unsigned int)x + w) > (unsigned int)this->m_w)
    {
         w = (unsigned int)(this->m_w - (unsigned int)x);
    }
    pages = ((y + h - 1) >> 3) - (y >> 3) + 1;

    uint8_t color = this->m_color ? 0xFF: 0x00;
    m_intf.startBlock(x, y >> 3, w);
    for(j=0; j < pages; j++)
    {
        if ( j == (unsigned int)(max_pages - 1) ) mainFlag = !offset;
        for( i=w; i > 0; i--)
        {
            uint8_t data = 0;
            if ( mainFlag )    data |= (( bitmap[0] << offset) & color);
            if ( complexFlag ) data |= (( (bitmap - origin_width)[0] >> (8 - offset)) & color);
            bitmap++;
            m_intf.send(s_ssd1306_invertByte^data);
        }
        bitmap += origin_width - w;
        complexFlag = offset;
        m_intf.nextBlock();
    }
    m_intf.endBlock();
}

template<>
uint8_t NanoDisplayOpsX<1>::printChar(uint8_t c)
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
size_t NanoDisplayOpsX<1>::write(uint8_t c)
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
void NanoDisplayOpsX<1>::printFixed_oldStyle(uint8_t xpos, uint8_t y, const char *ch, EFontStyle style)
{
    uint8_t i, j=0;
    uint8_t text_index = 0;
    uint8_t page_offset = 0;
    uint8_t x = xpos;
    y >>= 3;
    m_intf.startBlock(xpos, y, this->m_w - xpos);
    for(;;)
    {
        uint8_t c;
        uint8_t ldata;
        uint16_t offset;
        if( (x > this->m_w - this->m_font->getHeader().width) || (ch[j] == '\0') )
        {
            x = xpos;
            y++;
            if (y >= (this->m_h >> 3))
            {
                break;
            }
            page_offset++;
            if (page_offset == this->m_font->getPages())
            {
                text_index = j;
                page_offset = 0;
                if (ch[j] == '\0')
                {
                    break;
                }
            }
            else
            {
                j = text_index;
            }
            m_intf.endBlock();
            m_intf.startBlock(xpos, y, this->m_w - xpos);
        }
        c = ch[j];
        if ( c >= this->m_font->getHeader().ascii_offset )
        {
            c -= this->m_font->getHeader().ascii_offset;
        }
        ldata = 0;
        offset = (c * this->m_font->getPages() + page_offset) * this->m_font->getHeader().width;
        for( i=this->m_font->getHeader().width; i>0; i--)
        {
            uint8_t data;
            if ( style == STYLE_NORMAL )
            {
                data = this->m_font->getPrimaryTable()[offset];
            }
            else if ( style == STYLE_BOLD )
            {
                uint8_t temp = this->m_font->getPrimaryTable()[offset];
                data = temp | ldata;
                ldata = temp;
            }
            else
            {
                uint8_t temp = this->m_font->getPrimaryTable()[offset + 1];
                data = (temp & 0xF0) | ldata;
                ldata = (temp & 0x0F);
            }
            m_intf.send(data^s_ssd1306_invertByte);
            offset++;
        }
        x += this->m_font->getHeader().width;
        j++;
    }
    m_intf.endBlock();
}



template<>
void NanoDisplayOpsX<1>::printFixedN(int xpos, int y, const char *ch, EFontStyle style, uint8_t factor)
{
    uint8_t i, j=0;
    uint8_t text_index = 0;
    uint8_t page_offset = 0;
    uint8_t x = xpos;
    y >>= 3;
    m_intf.startBlock(xpos, y, this->m_w - xpos);
    for(;;)
    {
        uint8_t ldata;
        if( (x > this->m_w - (this->m_font->getHeader().width << factor)) || (ch[j] == '\0') )
        {
            x = xpos;
            y++;
            if (y >= (this->m_h >> 3))
            {
                break;
            }
            page_offset++;
            if (page_offset == (this->m_font->getPages() << factor))
            {
                text_index = j;
                page_offset = 0;
                if (ch[j] == '\0')
                {
                    break;
                }
            }
            else
            {
                j = text_index;
            }
            m_intf.endBlock();
            m_intf.startBlock(xpos, y, this->m_w - xpos);
        }
        uint16_t unicode;
        do
        {
            unicode = this->m_font->unicode16FromUtf8(ch[j]);
            j++;
        } while ( unicode == SSD1306_MORE_CHARS_REQUIRED );
        SCharInfo char_info;
        this->m_font->getCharBitmap(unicode, &char_info);
        ldata = 0;
        x += ((char_info.width + char_info.spacing) << factor);
        if (char_info.height > (page_offset >> factor) * 8)
        {
            char_info.glyph += (page_offset >> factor) * char_info.width;
            for( i=char_info.width; i>0; i--)
            {
                uint8_t data;
                if ( style == STYLE_NORMAL )
                {
                    data = char_info.glyph[0];
                }
                else if ( style == STYLE_BOLD )
                {
                    uint8_t temp = char_info.glyph[0];
                    data = temp | ldata;
                    ldata = temp;
                }
                else
                {
                    uint8_t temp = (char_info.glyph+1)[0];
                    data = (temp & 0xF0) | ldata;
                    ldata = (temp & 0x0F);
                }
                if ( factor > 0 )
                {
                    uint8_t accum = 0;
                    uint8_t mask = ~((0xFF) << (1<<factor));
                    // N=0  ->   right shift is always 0
                    // N=1  ->   right shift goes through 0, 4
                    // N=2  ->   right shift goes through 0, 2, 4, 6
                    // N=3  ->   right shift goes through 0, 1, 2, 3, 4, 5, 6, 7
                    data >>= ((page_offset & ((1<<factor) - 1))<<(3-factor));
                    for (uint8_t idx = 0; idx < 1<<(3-factor); idx++)
                    {
                         accum |= (((data>>idx) & 0x01) ? (mask<<(idx<<factor)) : 0);
                    }
                    data = accum;
                }
                for (uint8_t z=(1<<factor); z>0; z--)
                {
                    m_intf.send(data^s_ssd1306_invertByte);
                }
                char_info.glyph++;
            }
        }
        else
        {
            char_info.spacing += char_info.width;
        }
        for (i = 0; i < (char_info.spacing << factor); i++)
            m_intf.send(s_ssd1306_invertByte);
    }
    m_intf.stop();
}



template<>
void NanoDisplayOpsX<1>::putPixel(int x, int y)
{
    m_intf.startBlock(x, y >> 3, 1);
    m_intf.send((1 << (y & 0x07))^s_ssd1306_invertByte);
    m_intf.endBlock();
}



template<>
void NanoDisplayOpsX<1>::drawHLine(int x1, int y1, int x2)
{
    m_intf.startBlock(x1, y1 >> 3, x2 - x1 + 1);
    for (uint8_t x = x1; x <= x2; x++)
    {
        m_intf.send((1 << (y1 & 0x07))^s_ssd1306_invertByte);
    }
    m_intf.endBlock();
}



template<>
void NanoDisplayOpsX<1>::drawVLine(int x1, int y1, int y2)
{
    uint8_t topPage = y1 >> 3;
    uint8_t bottomPage = y2 >> 3;
    uint8_t height = y2-y1;
    uint8_t y;
    m_intf.startBlock(x1, topPage, 1);
    if (topPage == bottomPage)
    {
        m_intf.send( ((0xFF >> (0x07 - height)) << (y1 & 0x07))^s_ssd1306_invertByte );
        m_intf.endBlock();
        return;
    }
    m_intf.send( (0xFF << (y1 & 0x07))^s_ssd1306_invertByte );
    for ( y = (topPage + 1); y <= (bottomPage - 1); y++)
    {
        m_intf.nextBlock();
        m_intf.send( 0xFF^s_ssd1306_invertByte );
    }
    m_intf.nextBlock();
    m_intf.send( (0xFF >> (0x07 - (y2 & 0x07)))^s_ssd1306_invertByte );
    m_intf.endBlock();
}



template<>
void NanoDisplayOpsX<1>::fillRect(int x1, int y1, int x2, int y2)
{
    uint8_t templ = this->m_color^s_ssd1306_invertByte;
    if (x1 > x2) return;
    if (y1 > y2) return;
    if ((unsigned int)x2 >= this->m_w) x2 = (int)this->m_w -1;
    if ((unsigned int)y2 >= this->m_h) y2 = (int)this->m_h -1;
    uint8_t bank1 = (y1 >> 3);
    uint8_t bank2 = (y2 >> 3);
    m_intf.startBlock(x1, bank1, x2 - x1 + 1);
    for (uint8_t bank = bank1; bank<=bank2; bank++)
    {
        uint8_t mask = 0xFF;
        if (bank1 == bank2)
        {
            mask = (mask >> ((y1 & 7) + 7 - (y2 & 7))) << (y1 & 7);
        }
        else if (bank1 == bank)
        {
            mask = (mask << (y1 & 7));
        }
        else if (bank2 == bank)
        {
            mask = (mask >> (7 - (y2 & 7)));
        }
        for (uint8_t x=x1; x<=x2; x++)
        {
//            m_bytes[BADDR(bank) + x] &= ~mask;
//            m_bytes[BADDR(bank) + x] |= (templ & mask);
            m_intf.send(templ & mask);
        }
        m_intf.nextBlock();
    }
    m_intf.endBlock();
}


template<>
void NanoDisplayOpsX<1>::drawXBitmap(int x, int y, unsigned int w, unsigned int h, const uint8_t *bitmap)
{
    uint8_t i, j;
    unsigned int pitch = (w + 7) >> 3;
    m_intf.startBlock(x, y, w);
    for(j=(h >> 3); j>0; j--)
    {
        uint8_t bit = 0;
        for(i=w;i>0;i--)
        {
            uint8_t data = 0;
            for (uint8_t k = 0; k<8; k++)
            {
                data |= ( ( (bitmap[k*pitch] >> bit) & 0x01) << k );
            }
            m_intf.send( s_ssd1306_invertByte^data );
            bit++;
            if (bit >= 8)
            {
                bitmap++;
                bit=0;
            }
        }
        if (bit)
        {
            bitmap++;
        }
        bitmap += pitch * 7;
        m_intf.nextBlock();
    }
    m_intf.endBlock();
}






template<>
void NanoDisplayOpsX<1>::gfx_drawMonoBitmap(int x, int y, unsigned int w, unsigned int h, const uint8_t *buf)
{
    unsigned int origin_width = w;
    uint8_t offset = y & 0x07;
    uint8_t complexFlag = 0;
    uint8_t mainFlag = 1;
    uint8_t max_pages;
    uint8_t pages;
    unsigned int i, j;
    if (y + (int)h <= 0) return;
    if (y >= (int)this->m_h) return;
    if (x + (int)w <= 0) return;
    if (x >= (int)this->m_w)  return;
    if (y < 0)
    {
         buf += ((unsigned int)((-y) + 7) >> 3) * w;
         h += y;
         y = 0;
         complexFlag = 1;
    }
    if (x < 0)
    {
         buf += -x;
         w += x;
         x = 0;
    }
    max_pages = (unsigned int)(h + 15 - offset) >> 3;
    if ((unsigned int)((unsigned int)y + h) > (unsigned int)this->m_h)
    {
         h = (unsigned int)(this->m_h - (unsigned int)y);
    }
    if ((unsigned int)((unsigned int)x + w) > (unsigned int)this->m_w)
    {
         w = (unsigned int)(this->m_w - (unsigned int)x);
    }
    pages = ((y + h - 1) >> 3) - (y >> 3) + 1;

    uint8_t color = this->m_color ? 0xFF: 0x00;
    m_intf.startBlock(x, y >> 3, w);
    for(j=0; j < pages; j++)
    {
        if ( j == (unsigned int)(max_pages - 1) ) mainFlag = !offset;
        for( i=w; i > 0; i--)
        {
            uint8_t data = 0;
            if ( mainFlag )    data |= (( buf[0] << offset) & color);
            if ( complexFlag ) data |= (( (buf - origin_width)[0] >> (8 - offset)) & color);
            buf++;
            m_intf.send(s_ssd1306_invertByte^data);
        }
        buf += origin_width - w;
        complexFlag = offset;
        m_intf.nextBlock();
    }
    m_intf.endBlock();
}



template<>
void NanoDisplayOpsX<1>::drawBuffer1(int x, int y, unsigned int w, unsigned int h, const uint8_t *buffer)
{
    unsigned int origin_width = w;
    uint8_t offset = y & 0x07;
    uint8_t complexFlag = 0;
    uint8_t mainFlag = 1;
    uint8_t max_pages;
    uint8_t pages;
    unsigned int i, j;
    if (y + (int)h <= 0) return;
    if (y >= (int)this->m_h) return;
    if (x + (int)w <= 0) return;
    if (x >= (int)this->m_w)  return;
    if (y < 0)
    {
         buffer += ((unsigned int)((-y) + 7) >> 3) * w;
         h += y;
         y = 0;
         complexFlag = 1;
    }
    if (x < 0)
    {
         buffer += -x;
         w += x;
         x = 0;
    }
    max_pages = (unsigned int)(h + 15 - offset) >> 3;
    if ((unsigned int)((unsigned int)y + h) > (unsigned int)this->m_h)
    {
         h = (unsigned int)(this->m_h - (unsigned int)y);
    }
    if ((unsigned int)((unsigned int)x + w) > (unsigned int)this->m_w)
    {
         w = (unsigned int)(this->m_w - (unsigned int)x);
    }
    pages = ((y + h - 1) >> 3) - (y >> 3) + 1;

    m_intf.startBlock(x, y >> 3, w);
    for(j=0; j < pages; j++)
    {
        if ( j == (unsigned int)(max_pages - 1) ) mainFlag = !offset;
        for( i=w; i > 0; i--)
        {
            uint8_t data = 0;
            if ( mainFlag )    data |= ((*buffer << offset) & this->m_color);
            if ( complexFlag ) data |= ((*(buffer - origin_width) >> (8 - offset)) & this->m_color);
            buffer++;
            m_intf.send(s_ssd1306_invertByte^data);
        }
        buffer += origin_width - w;
        complexFlag = offset;
        m_intf.nextBlock();
    }
    m_intf.endBlock();
}



template<>
void NanoDisplayOpsX<1>::drawBuffer1Fast(int x, int y, unsigned int w, unsigned int h, const uint8_t *buf)
{
    uint8_t j;
    m_intf.startBlock(x, y >> 3, w);
    for(j=(h >> 3); j>0; j--)
    {
        m_intf.sendBuffer( buf, w );
        buf+=w;
        m_intf.nextBlock();
    }
    m_intf.endBlock();
}



template<>
void NanoDisplayOpsX<1>::drawBuffer4(int x, int y, unsigned int w, unsigned int h, const uint8_t *buffer)
{
    // NOT IMPLEMENTED
}



template<>
void NanoDisplayOpsX<1>::drawBuffer8(int x, int y, unsigned int w, unsigned int h, const uint8_t *buffer)
{
    // NOT IMPLEMENTED
}



template<>
void NanoDisplayOpsX<1>::drawBuffer16(int x, int y, unsigned int w, unsigned int h, const uint8_t *buffer)
{
    // NOT IMPLEMENTED
}



template<>
void NanoDisplayOpsX<1>::fill(uint16_t color)
{
    color ^= s_ssd1306_invertByte;
    m_intf.startBlock(0, 0, 0);
    for(unsigned int m=(this->m_h >> 3); m>0; m--)
    {
        for(unsigned int n=this->m_w; n>0; n--)
        {
            m_intf.send(color);
        }
        m_intf.nextBlock();
    }
    m_intf.endBlock();
}

