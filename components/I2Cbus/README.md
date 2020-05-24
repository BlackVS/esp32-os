# **I2Cbus extended**

I2C interface library in C++ for working with **Espressif ESP32 IoT Development Framework _(esp-idf)_** and other libriries, that supports master mode.

Based on https://github.com/natanaeljr/esp32-I2Cbus.git 

Extended to support lcdgfx (https://github.com/lexus2k/lcdgfx) I2C interface (i.e. now it combines two calling styles on single I2C bus)

Reason: some ESP32 boards have few devices in single I2C bus and usage both libraries at the same time becomes tricky.

As in the case of used here MPU and LCDGFX: both libraries use different, incompatible directly, styles for working with I2C. 

Original I2C class (https://github.com/natanaeljr/esp32-I2Cbus.git) was extended and adapted (i.e. some changes to the original logic were done) to support both calling approaches at the same time safely.

I.e. now usage is:
- install I2C driver with neded config ( i2c0.setup() + i2co.begin() )
- initialize MPU library using already installed driver ()
- initialize LCDGFX library using already installed driver (via special custom interface class)

Copyright © 2020 VVS/BlackVS
