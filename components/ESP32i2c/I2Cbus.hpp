/* =========================================================================
I2Cbus library is placed under the MIT License
Copyright 2017 Natanael Josue Rabello. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
 ========================================================================= */

/* =========================================================================
I2Cbus extended library is placed under the MIT License
Copyright 2020 Natanael Josue Rabello. All rights reserved.

*/

#pragma once

#include <stdint.h>
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_err.h"


/* ^^^^^^
 * I2Cbus
 * ^^^^^^ */
namespace i2cbus {
constexpr uint32_t kDefaultClockSpeed = 100000;  /*!< Clock speed in Hz, default: 100KHz */
constexpr uint32_t kDefaultTimeout = 1000;       /*!< Timeout in milliseconds, default: 1000ms */
class I2C;
}  // namespace i2cbus

// I2Cbus type
using I2C_t = i2cbus::I2C;

// Default Objects
//extern I2C_t i2c0;        /*!< port: I2C_NUM_0 */
//extern I2C_t i2c1;        /*!< port: I2C_NUM_1 */

// I2C class definition
namespace i2cbus {
   class I2C {

   public:
      explicit I2C(i2c_port_t port,
                  gpio_num_t sda_io_num, 
                  gpio_num_t scl_io_num,
                  uint32_t clk_speed          = kDefaultClockSpeed,
                  gpio_pullup_t sda_pullup_en = GPIO_PULLUP_ENABLE, 
                  gpio_pullup_t scl_pullup_en = GPIO_PULLUP_ENABLE);
      
      ~I2C();

      /////////////////////////////////////////////////////////////////
      // lcdgfx/EspI2c compatible interface
      /////////////////////////////////////////////////////////////////

      esp_err_t begin();
      esp_err_t end();
      void start(uint8_t addr);
      void stop();
      void send(uint8_t data);
      void sendBuffer(const uint8_t *buffer, uint16_t size);
      //void setAddr(uint8_t addr) { m_sa = addr; }
      void setTimeout(uint32_t ms);


      /////////////////////////////////////////////////////////////////
      // MPU/I2C compatible interface
      /////////////////////////////////////////////////////////////////

      /**
       * *** WRITING interface ***
       * @brief  I2C commands for writing to a 8-bit slave device register.
       *         All of them returns standard esp_err_t codes. So it can be used
       *         with ESP_ERROR_CHECK();
       * @param  devAddr   [I2C slave device register]
       * @param  regAddr   [Register address to write to]
       * @param  bitNum    [Bit position number to write to (bit 7~0)]
       * @param  bitStart  [Start bit number when writing a bit-sequence (MSB)]
       * @param  data      [Value(s) to be write to the register]
       * @param  length    [Number of bytes to write (should be within the data buffer size)]
       *                   [writeBits() -> Number of bits after bitStart (including)]
       * @param  timeout   [Custom timeout for the particular call]
       * @return  - ESP_OK Success
       *          - ESP_ERR_INVALID_ARG Parameter error
       *          - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
       *          - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
       *          - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.
       */
      esp_err_t writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data, int32_t timeout = -1);
      esp_err_t writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data, int32_t timeout = -1);
      esp_err_t writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data, int32_t timeout = -1);
      esp_err_t writeBytes(uint8_t devAddr, uint8_t regAddr, size_t length, const uint8_t *data, int32_t timeout = -1);

      /**
       * *** READING interface ***
       * @breif  I2C commands for reading a 8-bit slave device register.
       *         All of them returns standard esp_err_t codes.So it can be used
       *         with ESP_ERROR_CHECK();
       * @param  devAddr   [I2C slave device register]
       * @param  regAddr   [Register address to read from]
       * @param  bitNum    [Bit position number to write to (bit 7~0)]
       * @param  bitStart  [Start bit number when writing a bit-sequence (MSB)]
       * @param  data      [Buffer to store the read value(s)]
       * @param  length    [Number of bytes to read (should be within the data buffer size)]
       * @param  timeout   [Custom timeout for the particular call]
       * @return  - ESP_OK Success
       *          - ESP_ERR_INVALID_ARG Parameter error
       *          - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
       *          - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
       *          - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.]
       */
      esp_err_t readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data, int32_t timeout = -1);
      esp_err_t readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data, int32_t timeout = -1);
      esp_err_t readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, int32_t timeout = -1);
      esp_err_t readBytes(uint8_t devAddr, uint8_t regAddr, size_t length, uint8_t *data, int32_t timeout = -1);

      /**
       * @brief  Quick check to see if a slave device responds.
       * @param  devAddr   [I2C slave device register]
       * @param  timeout   [Custom timeout for the particular call]
       * @return  - ESP_OK Success
       *          - ESP_ERR_INVALID_ARG Parameter error
       *          - ESP_FAIL Sending command error, slave doesn't ACK the transfer.
       *          - ESP_ERR_INVALID_STATE I2C driver not installed or not in master mode.
       *          - ESP_ERR_TIMEOUT Operation timeout because the bus is busy.]
       */
      esp_err_t testConnection(uint8_t devAddr, int32_t timeout = -1);

      /**
       * I2C scanner utility, prints out all device addresses found on this I2C bus.
       */
      void scanner();

   private:
      i2c_port_t      m_port;          /*!< I2C port: I2C_NUM_0 or I2C_NUM_1 */
      gpio_num_t      m_sda_io_num;
      gpio_num_t      m_scl_io_num;
      uint32_t        m_clk_speed;
      gpio_pullup_t   m_scl_pullup_en;
      gpio_pullup_t   m_sda_pullup_en;
      uint32_t        m_ticksToWait;       /*!< Timeout in ticks for read and write */
      i2c_cmd_handle_t m_cmd_handle;
      //uint8_t         m_sa;
      bool            m_active = {false};

   }; // I2C class

}  // namespace i2cbus

