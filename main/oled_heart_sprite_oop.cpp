#include "app.h"

#ifdef BOARD_BADGE_NNC2019

#define TAG "OLED Heart OOP"

/*
 * Define sprite width. The width can be of any size.
 * But sprite height is always assumed to be 8 pixels
 * (number of bits in single byte).
 */
const int spriteWidth = sizeof(heartImage);

typedef NanoEngine1<DISPLAY_TYPE> ENGINE;
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
          if (y() >= static_cast<lcdint_t>(display.height()) )
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
    engine.getCanvas().clear();
    engine.getCanvas().drawBitmap1(0, 0, 128, 64, vvs_logo);
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

#endif