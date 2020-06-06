#include "app.h"

#define TAG "OLED Heart OOP"

/*
 * Define sprite width. The width can be of any size.
 * But sprite height is always assumed to be 8 pixels
 * (number of bits in single byte).
 */
const int spriteWidth = sizeof(heartImage);

typedef NanoEngine16<DISPLAY_TYPE> ENGINE;
ENGINE engine(display);
typedef NanoFixedSprite<ENGINE::TilerT> CSprite;


class CHeart: public CSprite
{
public:
    CHeart()
    : CSprite({0, 0}, {8, 8}, heartImage) 
    { 
    }

    bool isAlive() { return hasTiler();  }

    void respawn()
    {
        if(!isAlive())
        {
          /* Set initial position in scaled coordinates */
          scaled_position = { random( display.width() * 8), -8 * 8 };
          /* Use some random speed */
          speed = { random(-16, 16), random(4, 12) };
          /* After countdown timer ticks to 0, change X direction */
          moveTo( scaled_position/8 );
          engine.insert(*this);
        }
    }

    void update() override
    {
        if(isAlive())
        {
          scaled_position += speed;
          moveTo( scaled_position/8 );
          if (y() >= static_cast<int>(display.height()) )
          {
              destroyed();
          }
        }
    }

    void destroyed(void)
    {
        getTiler().remove(*this);
    }    

private:
    NanoPoint scaled_position;
    NanoPoint speed;
} heart;


bool onDraw()
{
    auto& canvas=engine.getCanvas();
    canvas.clear();
    canvas.setColor(RGB16_BLUE);
    canvas.fillRect(0,0, display.width()-1, display.height()/2-1);
    canvas.setColor(RGB16_YELLOW);
    canvas.fillRect(0,display.height()/2, display.width()-1, display.height()-1);
    //canvas.printFixed(0, 0, u8"UKRAINE!", STYLE_BOLD);
    canvas.setColor(RGB16_ORANGE);
    engine.getCanvas().drawBitmap1( (display.width()-128)/2, (display.height()-64)/4, 128, 64, vvs_logo);
    canvas.setColor(RGB16_RED);
    return true;
}

void loop_sprite()
{
    if (!engine.nextFrame()) {
      return;
    }
    if(!heart.isAlive()){
      heart.respawn();
    }
    engine.update();
    engine.display();
}

void setup_sprite()
{
    engine.getCanvas().setFixedFont(ssd1306xled_font8x16);
    engine.setFrameRate( 10 );
    engine.begin();
    engine.drawCallback( onDraw );
    engine.getCanvas().setMode(CANVAS_MODE_TRANSPARENT);
    engine.refresh();
}

void oled_heart_sprite_oop_task(void *pvParameters) 
{
  ESP_LOGD(__FUNCTION__, "Starting...");
  setup_sprite();
  while (1) {
    loop_sprite();
    vTaskDelay(5);
  }
  vTaskDelete(NULL); 
}
