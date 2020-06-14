#pragma once

#include "display_base.h"

template<uint8_t BPP>
class NanoDisplayOpsX: public NanoDisplayBase
{
public:
    /** number of bits per single pixel in buffer */
    const uint8_t BITS_PER_PIXEL = BPP;

    using NanoDisplayBase::NanoDisplayBase;

    /**
     * Draws pixel on specified position
     * @param x - position X
     * @param y - position Y
     * @note color can be set via setColor()
     */
    void putPixel(int x, int y);

    /**
     * Draws horizontal or vertical line
     * @param x1 - position X
     * @param y1 - position Y
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void drawVLine(int x1, int y1, int y2);

    /**
     * Draws horizontal or vertical line
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @note color can be set via setColor()
     */
    void drawHLine(int x1, int y1, int x2);

    /**
     * Fills rectangle area
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void fillRect(int x1, int y1, int x2, int y2);

    /**
     * Draws bitmap, located in Flash, on the display
     * The bitmap should be in XBMP format
     *
     * @param x - horizontal position in pixels
     * @param y - vertical position in blocks (pixels/8)
     * @param w - width of bitmap in pixels
     * @param h - height of bitmap in pixels (must be divided by 8)
     * @param bitmap - pointer to data, located in Flash: each byte represents 8 vertical pixels.
     */
    void drawXBitmap(int x, int y, unsigned int w, unsigned int h, const uint8_t *bitmap);

    /**
     * @brief Draws monochrome bitmap in color buffer using color, specified via setColor() method
     * Draws monochrome bitmap in color buffer using color, specified via setColor() method
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - monochrome bitmap data, located in flash
     *
     * @note There are 2 modes: transparent and non-transparent mode, - and 2 colors available: black and white.
     *       In non-transparent mode, when black color is selected, the monochrome image just inverted.
     *       In transparent mode, those pixels of source monochrome image, which are black, do not overwrite pixels
     *       in the screen buffer.
     */
    void drawBitmap1(int x, int y, unsigned int w, unsigned int h, const uint8_t *bitmap);

    /**
     * Draws bitmap, located in Flash, on the display
     *
     * @param x - horizontal position in pixels
     * @param y - vertical position in pixels
     * @param w - width of bitmap in pixels
     * @param h - height of bitmap in pixels (must be divided by 8)
     * @param buf - pointer to data, located in Flash: each byte represents 8 vertical pixels.
     * @warning only for monochrome displays
     */
    void gfx_drawMonoBitmap(int x, int y, unsigned int w, unsigned int h, const uint8_t *buf);

    /**
     * @brief Draws 4-bit gray-color bitmap in color buffer.
     * Draws 4-bit gray-color bitmap in color buffer.
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - 4-bit gray-color bitmap data, located in flash
     */
    void drawBitmap4(int x, int y, unsigned int w, unsigned int h, const uint8_t *bitmap);

    /**
     * @brief Draws 8-bit color bitmap in color buffer.
     * Draws 8-bit color bitmap in color buffer.
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - 8-bit color bitmap data, located in flash
     */
    void drawBitmap8(int x, int y, unsigned int w, unsigned int h, const uint8_t *bitmap);

    /**
     * Draw 16-bit color bitmap, located in Flash, directly to OLED display GDRAM.
     * Each pixel of the bitmap is expected in 5-6-5 format.
     *
     * @param xpos start horizontal position in pixels
     * @param ypos start vertical position in pixels
     * @param w bitmap width in pixels
     * @param h bitmap height in pixels
     * @param bitmap pointer to Flash data, containing 16-bit color bitmap.
     */
    void drawBitmap16(int xpos, int ypos, unsigned int w, unsigned int h, const uint8_t *bitmap);

    /**
     * Draws bitmap, located in RAM, on the display
     * Each byte represents 8 vertical pixels.
     *
     * ~~~~~~~~~~~~~~~{.cpp}
     * // Draw small rectangle 3x8 at position 10,8
     * uint8_t buffer[3] = { 0xFF, 0x81, 0xFF };
     * display.drawBuffer1(10, 1, 3, 8, buffer);
     * ~~~~~~~~~~~~~~~
     *
     * @param x horizontal position in pixels
     * @param y vertical position in pixels
     * @param w width of bitmap in pixels
     * @param h height of bitmap in pixels (must be divided by 8)
     * @param buffer pointer to data, located in SRAM: each byte represents 8 vertical pixels.
     */
    void drawBuffer1(int x, int y, unsigned int w, unsigned int h, const uint8_t *buffer);

    /**
     * Implements the same behavior as drawBuffer1, but much faster. This method has some limitations
     *
     * @param x horizontal position in pixels
     * @param y vertical position in pixels
     * @param w width of bitmap in pixels
     * @param h height of bitmap in pixels (must be divided by 8)
     * @param buffer pointer to data, located in SRAM: each byte represents 8 vertical pixels.
     */
    void drawBuffer1Fast(int x, int y, unsigned int w, unsigned int h, const uint8_t *buffer);

    /**
     * Draws 4-bit bitmap, located in RAM, on the display
     * Each byte represents two pixels in 4-4 format:
     * refer to GRAY_COLOR4 to understand scheme, being used.
     *
     * @param x horizontal position in pixels
     * @param y vertical position in pixels
     * @param w width of bitmap in pixels
     * @param h height of bitmap in pixels
     * @param buffer pointer to data, located in SRAM.
     */
    void drawBuffer4(int x, int y, unsigned int w, unsigned int h, const uint8_t *buffer);

    /**
     * Draws 8-bit bitmap, located in RAM, on the display
     * Each byte represents one pixel in 2-2-3 format:
     * refer to RGB_COLOR8 to understand RGB scheme, being used.
     *
     * @param x horizontal position in pixels
     * @param y vertical position in pixels
     * @param w width of bitmap in pixels
     * @param h height of bitmap in pixels
     * @param buffer pointer to data, located in SRAM.
     */
    void drawBuffer8(int x, int y, unsigned int w, unsigned int h, const uint8_t *buffer);

    /**
     * Draws 16-bit bitmap, located in RAM, on the display
     * Each pixel occupies 2 bytes (5-6-5 format): refer to RGB_COLOR16 to understand RGB scheme, being used.
     *
     * @param xpos horizontal position in pixels
     * @param ypos vertical position in pixels
     * @param w width of bitmap in pixels
     * @param h height of bitmap in pixels
     * @param buffer pointer to data, located in RAM.
     */
    void drawBuffer16(int xpos, int ypos, unsigned int w, unsigned int h, const uint8_t *buffer);


    /**
     * Fill screen content with specified color
     *
     * @param color color to fill display with
     */
    void fill(uint16_t color);

    /**
     * Clears canvas
     */
    void clear();

    /**
     * Draws single character to canvas
     * @param c - character code to print
     * @returns 0 if char is not printed
     */
    uint8_t printChar(uint8_t c);

    /**
     * Writes single character to canvas
     * @param c - character code to print
     */
    size_t write(uint8_t c);

    /**
     * Print text at specified position to canvas
     *
     * @param xpos  position in pixels
     * @param y     position in pixels
     * @param ch    pointer to NULL-terminated string.
     * @param style specific font style to use
     *
     * @note Supports only STYLE_NORMAL and STYLE_BOLD
     */
    void printFixed(int xpos, int y, const char *ch, EFontStyle style = STYLE_NORMAL);

    void printFixed_oldStyle(uint8_t xpos, uint8_t y, const char *ch, EFontStyle style);

    /**
     * Prints text to screen using size fixed font, scaled by factor value. <br>
     * Factor value 0 gives regular font size (6x8 for example) <br>
     * Factor value 1 gives double font size (12x16 if 6x8 font is used) <br>
     * Factor value 2 gives fourth font size (24x32 if 6x8 font is used) <br>
     * Factor value 3 gives eighth font size (48x64 if 6x8 font is used) <br>
     * @param xpos - horizontal position in pixels
     * @param y - vertical position in pixels
     * @param ch - NULL-terminated string to print
     * @param style - font style (EFontStyle), normal by default.
     * @param factor - 0, 1, 2, 3.
     * @returns number of chars in string
     * @see ssd1306_setFixedFont
     * @warning ssd1306_printFixed2x() can output chars at fixed y positions: 0, 8, 16, 24, 32, etc.
     *          If you specify [10,18], ssd1306_printFixed2x() will output text starting at [10,16] position.
     * @warning Be careful with you flash space! Do not mix too many different functions in single sketch.
     *          ssd1306_printFixedN() uses much flash: ~474 bytes, ssd1306_printFixed() needs 388 bytes.
     *          Placing both of these functions to your sketch will consume almost 1KiB.
     */
    void printFixedN(int xpos, int y, const char *ch, EFontStyle style, uint8_t factor);

protected:
};

#include "display_Xbits.inl"