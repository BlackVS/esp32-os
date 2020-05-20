#include "app.h"

#define TAG "DISPLAY"

class EspI2c_Custom
{
public:
    /**
     * Creates instance of i2c implementation for current platform.
     * @param config i2c platform configuration. Refer to SPlatformI2cConfig.
     */
    EspI2c_Custom(I2C_t& i2c, uint8_t addr)
        : m_i2c(i2c), m_addr(addr)
         {}

    /**
     * Initializes i2c interface
     */
    void begin()
    {
        m_i2c.begin();
    }


    /**
     * Closes i2c interface
     */
    void end()
    {
        m_i2c.end();
    }


    /**
     * Starts communication with SSD1306 display.
     */
    void start()
    {
        m_i2c.setAddr(m_addr);
        m_i2c.start();
    }

    /**
     * Ends communication with SSD1306 display.
     */
    void stop()
    {
        m_i2c.stop();
    }

    /**
     * Sends byte to SSD1306 device
     * @param data - byte to send
     */
    void send(uint8_t data)
    {
        m_i2c.send(data);
    }

    /**
     * @brief Sends bytes to SSD1306 device
     *
     * Sends bytes to SSD1306 device. This functions gives
     * ~ 30% performance increase than ssd1306_intf.send.
     *
     * @param buffer - bytes to send
     * @param size - number of bytes to send
     */
    void sendBuffer(const uint8_t *buffer, uint16_t size)
    {
        m_i2c.sendBuffer(buffer, size);
    }

    /**
     * Sets i2c address for communication
     * This API is required for some led displays having multiple
     * i2c addresses for different types of data.
     *
     * @param addr i2c address to set (7 bits)
     */
    void setAddr(uint8_t addr) 
    { 
        m_addr = addr; 
    }

protected:
    I2C_t&  m_i2c;
    uint8_t m_addr;

};

//Variant 1
//Problem - MPU and OLED on the same I2C interface and both try to install driver - as result error message
//DisplaySSD1306_128x64_I2C display(-1, {I2C_NUM_0, OLED_I2C_ADDR, OLED_I2C_SCL_IO, OLED_I2C_SDA_IO, OLED_I2C_FREQ_HZ} ); // {busId, addr, scl, sda, freq}

//Variant 2
//Force OLED use the I2C driver from MPU
//MPU I2C code extended to support both usage scenarios
DisplaySSD1306_128x64_CustomI2C<EspI2c_Custom> display(-1, i2c0, OLED_I2C_ADDR );

//DisplaySSD1306_128x64_SPI display(-1,{-1, 0, 1, 0, -1, -1); // Use this line for nano pi (RST not used, 0=CE, gpio1=D/C)
//DisplaySSD1306_128x64_SPI display(3,{-1, 4, 5, 0,-1,-1});   // Use this line for Atmega328p (3=RST, 4=CE, 5=D/C)
//DisplaySSD1306_128x64_SPI display(24,{-1, 0, 23, 0,-1,-1}); // Use this line for Raspberry  (gpio24=RST, 0=CE, gpio23=D/C)
//DisplaySSD1306_128x64_SPI display(22,{-1, 5, 21, 0,-1,-1}); // Use this line for ESP32 (VSPI)  (gpio22=RST, gpio5=CE for VSPI, gpio21=D/C)


void display_init(void)
{
    ESP_LOGD(__FUNCTION__, "Starting...");
    display.begin(); // will raise error on i2c init - it is normal
    display.setFixedFont(ssd1306xled_font6x8);
    display.clear();
    uint32_t x=16;
    uint32_t y=8;
    display.printFixed(x, y+=8, "===============", STYLE_NORMAL);
    display.printFixed(x, y+=8, " ESP OS by VVS ", STYLE_BOLD);
    display.printFixed(x, y+=8, "===============", STYLE_NORMAL);
    display.printFixed(x, y+=8, "  version " FIRMWARE_VERSION, STYLE_ITALIC);
    delay(2000);


    ESP_LOGD(__FUNCTION__, "Successfully started...");
}

