#include "app.h"

#define TAG "OLED Snow"

/*
 * Define snowflake images directly in flash memory.
 * This reduces SRAM consumption.
 * The image is defined from bottom to top (bits), from left to right (bytes).
 */
const PROGMEM uint8_t snowFlakeImage[8][8] =
{
    {
        0B00111000,
        0B01010100,
        0B10010010,
        0B11111110,
        0B10010010,
        0B01010100,
        0B00111000,
        0B00000000
    },
    {
        0B00010000,
        0B01010100,
        0B00111000,
        0B11101110,
        0B00111000,
        0B01010100,
        0B00010000,
        0B00000000
    },
    {
        0B00111000,
        0B00010000,
        0B10111010,
        0B11101110,
        0B10111010,
        0B00010000,
        0B00111000,
        0B00000000
    },
    {
        0B00011000,
        0B01011010,
        0B00100100,
        0B11011011,
        0B11011011,
        0B00100100,
        0B01011010,
        0B00011000
    },
    {
        0B00010000,
        0B00111000,
        0B01010100,
        0B11101110,
        0B01010100,
        0B00111000,
        0B00010000,
        0B00000000
    },
    {
        0B10000010,
        0B00101000,
        0B01101100,
        0B00010000,
        0B01101100,
        0B00101000,
        0B10000010,
        0B00000000
    },
    {
        0B01000100,
        0B10101010,
        0B01101100,
        0B00010000,
        0B01101100,
        0B10101010,
        0B01000100,
        0B00000000
    },
    {
        0B00101000,
        0B01010100,
        0B10111010,
        0B01101100,
        0B10111010,
        0B01010100,
        0B00101000,
        0B00000000
    },
};

typedef NanoEngine16<DISPLAY_TYPE> ENGINE;
ENGINE engine(display);
typedef NanoFixedSprite<ENGINE::TilerT> CSnowFlake;

class SnowFlake: public CSnowFlake
{
public:
    SnowFlake()
    : CSnowFlake({0, 0}, {8, 8}, nullptr) 
    { 
    }

    bool isAlive() 
    { 
        return hasTiler(); 
    }

    // void draw() override
    // {
    //     getTiler().getCanvas().setColor(RGB_COLOR8(64,255,255));
    //     CSnowFlake::draw();
    // }

    void generate()
    {
        int idx = random( sizeof(snowFlakeImage)/sizeof(snowFlakeImage[0]) );
        setBitmap( snowFlakeImage[idx] );
        /* Set initial position in scaled coordinates */
        scaled_position = { random( display.width() * 8), -8 * 8 };
        /* Use some random speed */
        speed = { random(-16, 16), random(4, 12) };
        /* After countdown timer ticks to 0, change X direction */
        timer = random(24, 48);
        moveTo( scaled_position/8 );
    }

    void update() override
    {
        scaled_position += speed;
        timer--;
        if (0 == timer)
        {
            /* Change movement direction */
            speed.x = random(-16, 16);
            timer = random(24, 48);
        }
        moveTo( scaled_position/8 );
        if (y() >= static_cast<int>( display.height()) )
        {
            getTiler().remove(*this);
        }
    }

private:
    NanoPoint scaled_position;
    NanoPoint speed;
    uint8_t timer;
    bool falling = false;
};

static const uint8_t maxCount = 20;

// These are our snow flakes
SnowFlake snowFlakes[maxCount];

bool onDraw()
{
    //ESP_LOGD(__FUNCTION__, "Starting...");
    auto& canvas=engine.getCanvas();
    canvas.clear();
    // canvas.setColor(RGB16_BLUE);
    // canvas.fillRect(0,0, display.width()-1, display.height()/2-1);
    // canvas.setColor(RGB16_YELLOW);
    // canvas.fillRect(0,display.height()/2, display.width()-1, display.height()-1);
    //canvas.printFixed(0, 0, u8"UKRAINE!", STYLE_BOLD);
    canvas.setColor(RGB16_ORANGE);
    canvas.drawBitmap1( (display.width()-128)/2, (display.height()-64)/4, 128, 64, vvs_logo);
    canvas.setColor(RGB16_WHITE);
    return true;
}

void setup_snow()
{
    engine.drawCallback( onDraw );
    engine.begin();
    engine.setFrameRate( 10 );
    engine.getCanvas().setMode(CANVAS_MODE_TRANSPARENT);
    engine.refresh();
}

void addSnowFlake()
{
    //ESP_LOGD(__FUNCTION__, "Starting...");
    for (uint8_t i=0; i<maxCount; i++)
    {
        if (!snowFlakes[i].isAlive())
        {
            snowFlakes[i].generate();
            engine.insert(snowFlakes[i]);
            break;
        }
    }
}


static uint8_t globalTimer=3;

void loop_snow()
{
    if (!engine.nextFrame()) 
    {
      return;
    }
    if (0 == (--globalTimer))
    {
        // Try to add new snowflake every ~ 90ms 
        globalTimer = 3;
        addSnowFlake();
    }
    //ESP_LOGD(__FUNCTION__, "update...");    
    engine.update();
    engine.display();
}

void oled_snow_task(void *pvParameters) 
{
  ESP_LOGD(__FUNCTION__, "Starting...");
  setup_snow();
  display.clear();

  while (1) {
    loop_snow();
    vTaskDelay(10);
  }
  vTaskDelete(NULL); 
}

