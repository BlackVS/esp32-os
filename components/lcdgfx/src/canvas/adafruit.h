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
/**
 * @file adafruit.h Adafruit related canvas implementation
 *
 * @brief Canvas implementation, based on Adafruit GFX.
 *
 * @details If you like canvas implementation by Adafruit, you can easily use it with
 *          the library, including NanoEngine support. You will be able to use
 *          all features of AdafruitGFX, and output result to any OLED display, supported
 *          by the library. If you want to use this feature, define CONFIG_ADAFRUIT_GFX_ENABLE
 *          in the beginning of your sketch, and include "nano_engine.h" header.
 */
#pragma once

#include "platform.h"

#if defined(CONFIG_ADAFRUIT_GFX_ENABLE)

#include "nano_gfx_types.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/* This is special case for non-Arduino platforms, since Adafruit requires *
 * Arduino libraries support                                               */
#ifndef ARDUINO
#define ARDUINO 100
#include "Adafruit_GFX.h"
#undef ARDUINO
#else
#include "Adafruit_GFX.h"
#endif

#endif // DOXYGEN_SHOULD_SKIP_THIS

/**
 * @ingroup NANO_ENGINE_API_V2
 * @{
 */

/**
 * This is basic template class for all canvas classes,
 * based on Adafruit_GFX. This base class provides functionality
 * compatible with native NanoCanvas implementation of the
 * library
 */
template <uint8_t BPP>
class AdafruitCanvasOps: public Adafruit_GFX
{
public:
    /** Fixed offset for all operation of NanoCanvasOps in pixels. */
    NanoPoint offset;

    /** number of bits per single pixel in buffer */
    static const uint8_t BITS_PER_PIXEL = BPP;

    /**
     * Initializes canvas, based on Adafruit GFX.
     * @param w width of canvas
     * @param h height of canvas area
     * @param buffer buffer to use for pixels
     *
     * @note the size of buffer must be enough to store (w*h*bpp/8) bytes.
     */
    AdafruitCanvasOps(unsigned int w, unsigned int h, uint8_t *buffer)
        : Adafruit_GFX(w, h)
        , offset{0}
        , m_buffer(buffer)
    {
    }

    /**
     * draw single pixel in canvas area
     *
     * @param x x position
     * @param y y position
     * @param color color of pixel: for monochrome it can be 0 (black),
     *        1 (white), 2 (invert)
     */
    void drawPixel(int16_t x, int16_t y, uint16_t color) override;

    /**
     * Sets offset
     * @param ox - X offset in pixels
     * @param oy - Y offset in pixels
     */
    void setOffset(int ox, int oy) { offset.x = ox; offset.y = oy; };

#ifndef DOXYGEN_SHOULD_SKIP_THIS
    // We need to override Adafruit GFX implementation of fillScreen, because
    // NanoEngine uses offsets, when refreshing screen content.
    void fillScreen(uint16_t color) override
    {
        fillRect(offset.x, offset.y, _width, _height, color);
    }
#endif

protected:

    /** pixels buffer */
    uint8_t *m_buffer;

private:
    inline void rotatePosition(int16_t &x, int16_t &y)
    {
        switch (getRotation()) {
        case 1:
            canvas_swap_data(x, y, int16_t);
            x = WIDTH - x - 1;
            break;
        case 2:
            x = WIDTH - x - 1;
            y = HEIGHT - y - 1;
            break;
        case 3:
            canvas_swap_data(x, y, int16_t);
            y = HEIGHT - y - 1;
            break;
        }

    }
};

/**
 * Base class for all AdafruitCanvas childs
 */
template <uint8_t BPP>
class AdafruitCanvasBase: public AdafruitCanvasOps<BPP>
{
public:
    using AdafruitCanvasOps<BPP>::AdafruitCanvasOps;

    /**
     * Draws canvas on the LCD display
     * @param x - horizontal position in pixels
     * @param y - vertical position in blocks (pixels/8)
     */
    virtual void blt(int x, int y) = 0;

    /**
     * Draws canvas on the LCD display using offset values.
     */
    virtual void blt() = 0;
};

/////////////////////////////////////////////////////////////////////////////////
//
//                             1-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

/**
 * AdafruitCanvas1 represents objects for drawing in memory buffer
 * AdafruitCanvas1 represents each pixel as single bit: 0/1
 */
class AdafruitCanvas1 : public AdafruitCanvasBase<1>
{
public:
    using AdafruitCanvasBase::AdafruitCanvasBase;
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <>
void AdafruitCanvasOps<1>::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    x -= offset.x;
    y -= offset.y;
    if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    {
        return;
    }
    rotatePosition(x, y);

    switch (color)
    {
        case 1:   m_buffer[x+ (y/8)*WIDTH] |=  (1 << (y&7)); break;
        case 0:   m_buffer[x+ (y/8)*WIDTH] &= ~(1 << (y&7)); break;
        case 2:   m_buffer[x+ (y/8)*WIDTH] ^=  (1 << (y&7)); break;
    }
}
#endif // DOXYGEN_SHOULD_SKIP_THIS

/////////////////////////////////////////////////////////////////////////////////
//
//                             8-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

/**
 * AdafruitCanvas8 represents objects for drawing in memory buffer
 * AdafruitCanvas8 represents each pixel as single byte with RGB bits: RRRGGGBB
 * For details refer to SSD1331 datasheet.
 */
class AdafruitCanvas8 : public AdafruitCanvasBase<8>
{
public:
    using AdafruitCanvasBase::AdafruitCanvasBase;
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <>
void AdafruitCanvasOps<8>::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    x -= offset.x;
    y -= offset.y;
    if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    {
        return;
    }
    rotatePosition(x, y);

    m_buffer[x+y*WIDTH] = color;
}
#endif // DOXYGEN_SHOULD_SKIP_THIS

/////////////////////////////////////////////////////////////////////////////////
//
//                             16-BIT GRAPHICS
//
/////////////////////////////////////////////////////////////////////////////////

/**
 * AdafruitCanvas16 represents objects for drawing in memory buffer
 * AdafruitCanvas16 represents each pixel as two bytes with RGB bits:
 * RRRRRGGG GGGBBBBB.
 * For details refer to SSD1351 datasheet.
 */
class AdafruitCanvas16 : public AdafruitCanvasBase<16>
{
public:
    using AdafruitCanvasBase::AdafruitCanvasBase;
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <>
void AdafruitCanvasOps<16>::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    x -= offset.x;
    y -= offset.y;
    if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    {
        return;
    }
    rotatePosition(x, y);

    m_buffer[(x+y*WIDTH) * 2 + 0] = color;
    m_buffer[(x+y*WIDTH) * 2 + 1] = color >> 8;
}
#endif // DOXYGEN_SHOULD_SKIP_THIS

/**
 * @}
 */

#endif // CONFIG_ADAFRUIT_GFX_ENABLE

