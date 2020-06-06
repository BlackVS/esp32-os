/*
    MIT License

    Copyright (c) 2017-2020, Alexey Dynda

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
 * @file canvas/canvas_types.h Basic structures of canvas gfx library
 */

#pragma once

#include "canvas/UserSettings.h"

#if defined(ARDUINO)
#include <Arduino.h>
#elif defined(__AVR__)
#include <avr/pgmspace.h>
#else
#endif

#include <cstdint>
#include <stddef.h>

/** Flag means that more chars are required to decode utf-8 */
#define SSD1306_MORE_CHARS_REQUIRED  0xffff

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#ifndef PROGMEM
#define PROGMEM
#endif
#endif

/** Macro to generate 8-bit color */
#define RGB_COLOR8(r,g,b)    ( (r & 0xE0) | ((g >> 3)&0x1C) | (b>>6) )

/** Macro to generate 4-bit monochrome color from gray component */
#define GRAY_COLOR4(gray)    ( ((gray >> 4) & 0x0F) | (gray & 0xF0) )

/** Macro to generate 4-bit monochrome color from RGB */
#define RGB_COLOR4(r,g,b)    ( (r >> 2) + (g >> 1) + (b >> 2) )

/** Macro to convert 8-bit RGB to 4-bit monochrome format */
#define RGB8_TO_GRAY4(rgb)  ( (rgb >> 6) + ((rgb >> 2) & 0x07) + (rgb & 0x03) )

/** Macro to generate 16-bit RGB color */
#define RGB_COLOR16(r,g,b)   ( ((r<<8) & 0xF800) | ((g << 3)&0x07E0) | (b>>3) )

/** Macro to convert 3-3-2 color to 5-6-5 color */
#define RGB8_TO_RGB16(c)     ( (((uint16_t)c & 0b11100000) << 8) | \
                               (((uint16_t)c & 0b00011100) << 6) | \
                               (((uint16_t)c & 0b00000011) << 3) )

/** Macro to convert 5-6-5 color to 3-3-2 color */
#define RGB16_TO_RGB8(c)     ( ((uint16_t)(c >> 8) & 0b11100000) | \
                               ((uint16_t)(c >> 6) & 0b00011100) | \
                               ((uint16_t)(c >> 3) & 0b00000011) )


constexpr uint16_t RGB16_BLACK      = 0x0000;      /*   0,   0,   0 */
constexpr uint16_t RGB16_NAVY       = 0x000F;      /*   0,   0, 128 */
constexpr uint16_t RGB16_DARKGREEN  = 0x03E0;      /*   0, 128,   0 */
constexpr uint16_t RGB16_DARKCYAN   = 0x03EF;      /*   0, 128, 128 */
constexpr uint16_t RGB16_MAROON     = 0x7800;      /* 128,   0,   0 */
constexpr uint16_t RGB16_PURPLE     = 0x780F;      /* 128,   0, 128 */
constexpr uint16_t RGB16_OLIVE      = 0x7BE0;      /* 128, 128,   0 */
constexpr uint16_t RGB16_LIGHTGREY  = 0xC618;      /* 192, 192, 192 */
constexpr uint16_t RGB16_DARKGREY   = 0x7BEF;      /* 128, 128, 128 */
constexpr uint16_t RGB16_BLUE       = 0x001F;      /*   0,   0, 255 */
constexpr uint16_t RGB16_GREEN      = 0x07E0;      /*   0, 255,   0 */
constexpr uint16_t RGB16_CYAN       = 0x07FF;      /*   0, 255, 255 */
constexpr uint16_t RGB16_RED        = 0xF800;      /* 255,   0,   0 */
constexpr uint16_t RGB16_MAGENTA    = 0xF81F;      /* 255,   0, 255 */
constexpr uint16_t RGB16_YELLOW     = 0xFFE0;      /* 255, 255,   0 */
constexpr uint16_t RGB16_WHITE      = 0xFFFF;      /* 255, 255, 255 */
constexpr uint16_t RGB16_ORANGE     = 0xFD20;      /* 255, 165,   0 */
constexpr uint16_t RGB16_GREENYELLOW= 0xAFE5;      /* 173, 255,  47 */
constexpr uint16_t RGB16_PINK       = 0xF81F;

/** Supported font styles */
typedef enum
{
    STYLE_NORMAL,
    STYLE_BOLD,
    STYLE_ITALIC,
} EFontStyle;

enum
{
    CANVAS_MODE_BASIC           = 0x00,
    /** If the flag is specified, text cursor is moved to new line when end of screen is reached */
    CANVAS_TEXT_WRAP            = 0x01,
    /** This flag make bitmaps transparent (Black color) */
    CANVAS_MODE_TRANSPARENT     = 0x02,
    /** If the flag is specified, text cursor is moved to new line when end of canvas is reached */
    CANVAS_TEXT_WRAP_LOCAL      = 0x04,
};

/** Supported scale font values */
typedef enum
{
    FONT_SIZE_NORMAL = 0,
    FONT_SIZE_2X     = 1,
    FONT_SIZE_4X     = 2,
    FONT_SIZE_8X     = 3,
} EFontSize;

#pragma pack(push, 1)
/** Structure describes font format in memory */
typedef struct
{
    uint8_t type;         ///< font type: 0 - Fixed Font
    uint8_t width;        ///< width in pixels
    uint8_t height;       ///< height in pixels
    uint8_t ascii_offset; ///< ascii offset
} SFontHeaderRecord;
#pragma pack(pop)

/** Structure is used for internal font presentation */
typedef struct
{
    SFontHeaderRecord h; ///< record, containing information on font
    uint8_t count; ///< count of characters
    uint8_t pages; ///< height in pages (each page height is 8-pixels)
    uint8_t glyph_size;  ///< glyph size in bytes
    uint8_t spacing; ///< spacing between two characters
    const uint8_t *primary_table; ///< font chars bits
#ifdef CONFIG_SSD1306_UNICODE_ENABLE
    const uint8_t *secondary_table; ///< font chars bits
#endif
} SFixedFontInfo;

/** Structure describes single char information */
typedef struct
{
    uint8_t width;      ///< char width in pixels
    uint8_t height;     ///< char height in pixels
    uint8_t spacing;      ///< additional spaces after char in pixels
    const uint8_t *glyph; ///< char data, located in progmem.
} SCharInfo;

/**
 * Describes menu object
 */
typedef struct
{
    /// list of menu items of the menu
    const char **items;
    /// count of menu items in the menu
    uint8_t     count;
    /// currently selected item. Internally updated.
    uint8_t     selection;
    /// selected item, when last redraw operation was performed. Internally updated.
    uint8_t     oldSelection;
    /// position of menu scrolling. Internally updated
    uint8_t     scrollPosition;
    /// top offset
    int    top;
    /// left offset
    int    left;
    /// width of menu
    unsigned int   width;
    /// height of menu
    unsigned int   height;
} SAppMenu;

