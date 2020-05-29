# This is Makefile for ESP32 IDF

COMPONENT_ADD_INCLUDEDIRS := ./src \

COMPONENT_SRCDIRS := ./src \
                     ./src/lcd_hal \
                     ./src/lcd_hal/esp \
                     ./src/v2 \
                     ./src/v2/lcd \
                     ./src/v2/lcd/ssd1306 \
                     ./src/v2/lcd/st7735 \
                     ./src/canvas \
                     ./src/canvas/fonts \
                     ./src/v2/nano_engine \
