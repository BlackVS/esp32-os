#pragma once

#include "lcd_hal/io.h"
#include "v2/lcd/lcd_common.h"
#include "v2/lcd/base/display.h"

/**
 * @ingroup LCD_INTERFACE_API_V2
 * @{
 */

/**
 * Class implements interface functions to SSD1306 displays
 */
template <class I>
class InterfaceSSD1306: public I
{
public:
    /**
     * Creates instance of interface to LCD display.
     *
     * @param base Reference to base class, which represents Display
     * @param dc Data/command control pin number, for i2c communication should be -1
     * @param data variable argument list, accepted by platform interface (PlatformI2c, PlatformSpi)
     */
    template <typename... Args>
    InterfaceSSD1306(NanoDisplayBase<InterfaceSSD1306<I>> &base, int8_t dc, Args&&... data)
        : I(data...)
        , m_dc( dc )
        , m_base(base)
    {
    }

    /**
     * @brief Sets block in RAM of lcd display controller to write data to.
     *
     * Sets block in RAM of lcd display controller to write data to.
     * For SSD1306 it uses horizontal addressing mode, while for
     * sh1106 the function uses page addressing mode.
     * Width can be specified as 0, thus the library will set the right boundary to
     * region of RAM block to the right column of the display.
     * @param x - column (left region)
     * @param y - row (top region)
     * @param w - width of the block in pixels to control
     *
     * @warning - this function initiates session (i2c or spi) and does not close it.
     *            To close session, please, call endBlock().
     */
    void startBlock(lcduint_t x, lcduint_t y, lcduint_t w);

    /**
     * Switches to the start of next RAM page for the block, specified by
     * startBlock().
     * For SSD1306 it does nothing, while for sh1106 the function moves cursor to
     * next page.
     */
    void nextBlock();

    /**
     * Closes data send operation to lcd display.
     */
    void endBlock();

    /**
     * Enables either data or command mode on SPI bus
     * @param mode 1 to enable data mode, or 0 to enable command mode
     */
    void spiDataMode(uint8_t mode);

    /**
     * Starts communication with LCD display in command mode.
     * To stop communication use m_intf.end().
     */
    void commandStart();

    /**
     * Sets start line in gdram to start display content with
     *
     * @param line start line in range 0 - 63
     */
    void setStartLine(uint8_t line);

    /**
     * returns start line in gdram.
     */
    uint8_t getStartLine();

    /**
     * Switches display to normal mode. This feature is specific
     * for SSD1306 based controllers.
     */
    void normalMode();

    /**
     * Switches display to normal mode. This feature is specific
     * for SSD1306 based controllers.
     */
    void invertMode();

    /**
     * Set display contrast, ie light intensity
     * @param contrast - contrast value to see, refer to SSD1306 datasheet
     */
    void setContrast(uint8_t contrast);

    /**
     * Turns off display
     */
    void displayOff();

    /**
     * Turns on display
     */
    void displayOn();

    /**
     * @brief performs horizontal flip
     *
     * Performs horizontal flip. If you need to turn display by 180 degree,
     * please use both flipHorizontal() and flipVertical().
     *
     * @param mode - 0 to disable horizontal flip
     *               1 to enable horizontal flip
     */
    void flipHorizontal(uint8_t mode);

    /**
     * @brief performs vertical flip
     *
     * Performs vertical flip. If you need to turn display by 180 degree,
     * please use both flipHorizontal() and flipVertical().
     *
     * @param mode - 0 to disable vertical flip
     *               1 to enable vertical flip
     */
    void flipVertical(uint8_t mode);

private:
    int8_t m_dc = -1; ///< data/command pin for SPI, -1 for i2c
    NanoDisplayBase<InterfaceSSD1306<I>> &m_base; ///< basic lcd display support interface
    uint8_t m_startLine = 0;
};


/**
 * Class implements basic functions for 1-bit mode of SSD1306-based displays
 */
template <class I>
class DisplaySSD1306: public NanoDisplayOps<NanoDisplayOps1<I>,I>
{
public:
    /**
     * Creates instance of SSD1306 controller class for 1-bit mode
     *
     * @param intf interface to use
     * @param rstPin pin to use as HW reset pin for LCD display
     */
    DisplaySSD1306(I &intf, int8_t rstPin)
        : NanoDisplayOps<NanoDisplayOps1<I>, I>(intf)
        , m_rstPin( rstPin ) { }

protected:
    int8_t m_rstPin; ///< indicates hardware reset pin used, -1 if it is not required

    /**
     * Basic SSD1306 initialization
     */
    void begin() override;

    /**
     * Basic SSD1306 deinitialization
     */
    void end() override;
};

/**
 * Class implements basic functions for 1-bit mode of SSD1306-based displays
 */
template <class I>
class DisplaySSD1306_128x64: public DisplaySSD1306<I>
{
public:
    /**
     * Creates instance of SSD1306 128x64 controller class for 1-bit mode
     *
     * @param intf interface to use
     * @param rstPin pin to use as HW reset pin for LCD display
     */
    DisplaySSD1306_128x64(I &intf, int8_t rstPin)
        : DisplaySSD1306<I>(intf, rstPin) { }

protected:

    /**
     * Basic SSD1306 128x64 initialization
     */
    void begin() override;

    /**
     * Basic SSD1306 deinitialization
     */
    void end() override;
};

/**
 * Template class implements SSD1306 128x64 lcd display in 1 bit mode over custom I2C implementation
 * (user-defined i2c implementation). I - user custom i2c class
 */
template <class I>
class DisplaySSD1306_128x64_CustomI2C: public DisplaySSD1306_128x64<InterfaceSSD1306<I>>
{
public:
    /**
     * @brief Inits 128x64 lcd display over i2c (based on SSD1306 controller): 1-bit mode.
     *
     * Inits 128x64 lcd display over i2c (based on SSD1306 controller): 1-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param data variable argument list for custom user i2c interface.
     */
    template <typename... Args>
    DisplaySSD1306_128x64_CustomI2C( int8_t rstPin, Args&&... data )
        : DisplaySSD1306_128x64<InterfaceSSD1306<I>>(m_i2c, rstPin)
        , m_i2c( *this, -1,
                 data... ) {}

    /**
     * Initializes SSD1306 lcd in 1-bit mode
     */
    void begin() override
    {
        m_i2c.begin();
        DisplaySSD1306_128x64<InterfaceSSD1306<I>>::begin();
    }

    /**
     * Closes connection to display
     */
    void end() override
    {
        DisplaySSD1306_128x64<InterfaceSSD1306<I>>::end();
        m_i2c.end();
    }

private:
    InterfaceSSD1306<I> m_i2c;
};


#include "lcd_ssd1306.inl"

/**
 * @}
 */

