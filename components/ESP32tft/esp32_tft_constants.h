#pragma once



typedef enum {
 ST7735_INITR_GREENTAB       = 0x0,
 ST7735_INITR_REDTAB         = 0x1,
 ST7735_INITR_BLACKTAB       = 0x2, // Display with no offsets
 ST7735_INITR_GREENTAB2      = 0x3, // Use if you get random pixels on two edges of green tab display
 ST7735_INITR_GREENTAB3      = 0x4, // Use if you get random pixels on edge(s) of 128x128 screen
 ST7735_INITR_GREENTAB128    = 0x5, // Use if you only get part of 128x128 screen in rotation 0 & 1
 ST7735_INITR_GREENTAB160x80 = 0x6, // Use if you only get part of 128x128 screen in rotation 0 & 1
 ST7735_INITR_REDTAB160x80   = 0x7, // Added for https://www.aliexpress.com/item/ShengYang-1pcs-IPS-0-96-inch-7P-SPI-HD-65K-Full-Color-OLED-Module-ST7735-Drive/32918394604.html
 ST7735_INITB                = 0xB
} ST7735_INIT2_TYPE;


// Color definitions for backwards compatibility with old sketches
// use colour definitions like TFT_BLACK to make sketches more portable
#define ST7735_BLACK       0x0000      /*   0,   0,   0 */
#define ST7735_NAVY        0x000F      /*   0,   0, 128 */
#define ST7735_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ST7735_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ST7735_MAROON      0x7800      /* 128,   0,   0 */
#define ST7735_PURPLE      0x780F      /* 128,   0, 128 */
#define ST7735_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ST7735_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ST7735_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ST7735_BLUE        0x001F      /*   0,   0, 255 */
#define ST7735_GREEN       0x07E0      /*   0, 255,   0 */
#define ST7735_CYAN        0x07FF      /*   0, 255, 255 */
#define ST7735_RED         0xF800      /* 255,   0,   0 */
#define ST7735_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ST7735_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ST7735_WHITE       0xFFFF      /* 255, 255, 255 */
#define ST7735_ORANGE      0xFD20      /* 255, 165,   0 */
#define ST7735_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ST7735_PINK        0xF81F

// ST7735 specific commands used in init
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B // PASET
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
//Add
#define ST7735_VSCRDEF 0x33
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36
#define ST7735_VSCRSADD 0x37

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Delay between some initialisation commands
#define TFT_INIT_DELAY 0x80


// Generic commands used by TFT_eSPI.cpp
#define TFT_NOP     0x00
#define TFT_SWRST   0x01

#define TFT_CASET   0x2A
#define TFT_PASET   0x2B
#define TFT_RAMWR   0x2C

#define TFT_RAMRD   0x2E
#define TFT_IDXRD   0x00 //0xDD // ILI9341 only, indexed control register read

#define TFT_MADCTL  0x36
#define TFT_MAD_MY  0x80
#define TFT_MAD_MX  0x40
#define TFT_MAD_MV  0x20
#define TFT_MAD_ML  0x10
#define TFT_MAD_BGR 0x08
#define TFT_MAD_MH  0x04
#define TFT_MAD_RGB 0x00

#define TFT_INVOFF  0x20
#define TFT_INVON   0x21




//https://exploreembedded.com/wiki/OLED_Interface_With_8051
//https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
#define SSD1306_COMMAND             0x00		//This command is used to indicate the next data byte is acted as a command.
#define SSD1306_MEMORY_ADDR_MODE    0x20		//If horizontal address increment mode is enabled by command 20h, after finishing read/write one column data, it is incremented automatically to the next column address.
#define SSD1306_SET_COLUMN_ADDR     0x21		//This command is used to define the current read/write column address in graphic display data RAM.
#define SSD1306_SET_PAGE_ADDR       0x22		//This command is used to define the current read/write Line(Page as per data sheet) address in graphic display data RAM.
#define SH1106_SET_VPP              0x33
#define SSD1306_SETSTARTLINE        0x40
#define SSD1306_SETCONTRAST         0x81		//This command sets the Contrast Setting of the display. The chip has 256 contrast steps from 00h to FFh. The segment output current increases as the contrast step value increases.
#define SSD1306_CHARGEPUMP          0x8D
#define SSD1306_SEGREMAP            0xA0
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON        0xA5
#define SSD1306_NORMAL_DISPLAY      0xA6		//This command sets the display to normal mode. In normal display a RAM data of 1 indicates an “ON” pixel.
#define SSD1306_INVERT_DISPLAY      0xA7		//This command sets the display to inverse mode. In normal display a RAM data of 0 indicates an “ON” pixel.
#define SSD1306_SETMULTIPLEX        0xA8
#define SSD1306_DISPLAY_OFF         0xAE		//This command is used to turn OFF the OLED display panel.
#define SSD1306_DISPLAY_ON          0xAF		//This command is used to turn ON the OLED display panel.
#define SSD1306_COMSCANINC          0xC0        //normal mode (RESET) Scan fromCOM0 to COM[N –1] 
#define SSD1306_COMSCANDEC          0xC8        //remapped mode. Scan from COM[N-1] to COM0 
#define SSD1306_SETDISPLAYOFFSET    0xD3
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5
#define SSD1306_SETPRECHARGE        0xD9
#define SSD1306_SETCOMPINS          0xDA
#define SSD1306_SETVCOMDETECT       0xDB


#define SSD1306_DATA_CONTINUE       0x40		//This command sets the Display Start Line register to determine starting address of display RAM

#define SSD1306_SETLOWCOLUMN    0x00
#define SSD1306_SETHIGHCOLUMN   0x10

#define SH1106_SETPAGEADDRESS   0xB0






#define SH1106_SETCONTRAST 0x81
#define SH1106_DISPLAYALLON_RESUME 0xA4
#define SH1106_DISPLAYALLON 0xA5
#define SH1106_NORMALDISPLAY 0xA6
#define SH1106_INVERTDISPLAY 0xA7
#define SH1106_DISPLAYOFF 0xAE
#define SH1106_DISPLAYON 0xAF

#define SH1106_SETDISPLAYOFFSET 0xD3
#define SH1106_SETCOMPINS 0xDA

#define SH1106_SETVCOMDETECT 0xDB

#define SH1106_SETDISPLAYCLOCKDIV 0xD5
#define SH1106_SETPRECHARGE 0xD9

#define SH1106_SETMULTIPLEX 0xA8

#define SH1106_SETLOWCOLUMN 0x02 //to use with SSD1306, set to 0x00
#define SH1106_SETHIGHCOLUMN 0x10

#define SH1106_SETSTARTLINE 0x40

#define SH1106_MEMORYMODE 0x20
#define SH1106_COLUMNADDR 0x21
#define SH1106_PAGEADDR   0x22

#define SH1106_COMSCANINC 0xC0
#define SH1106_COMSCANDEC 0xC8

#define SH1106_SEGREMAP 0xA0

#define SH1106_CHARGEPUMP 0x8D

#define SH1106_EXTERNALVCC 0x1
#define SH1106_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SH1106_ACTIVATE_SCROLL 0x2F
#define SH1106_DEACTIVATE_SCROLL 0x2E
#define SH1106_SET_VERTICAL_SCROLL_AREA 0xA3
#define SH1106_RIGHT_HORIZONTAL_SCROLL 0x26
#define SH1106_LEFT_HORIZONTAL_SCROLL 0x27
#define SH1106_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SH1106_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A