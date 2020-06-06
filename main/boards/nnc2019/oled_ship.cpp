#include "app.h"


#define TAG "OLED Ship"


typedef NanoEngine1<DISPLAY_TYPE> ENGINE;
ENGINE engine(display);
typedef NanoFixedSprite<ENGINE::TilerT> CSprite;

class CAsteroid: public CSprite
{
public:
    CAsteroid()
    : CSprite({0, 0}, {8, 8}, nullptr) 
    { 
    }

    bool isAlive() { return hasTiler();  }

    void bringToLife()
    {
        if(!isAlive())
        {
            int idx = random( sizeof(AsteroidsImage)/sizeof(AsteroidsImage[0]) );
            setBitmap( AsteroidsImage[idx] );
            /* Set initial position in scaled coordinates */
            scaled_position = { random(display.width() * 8), -8 * 8 };
            /* Use some random speed */
            speed = { random(-16, 16), random(4, 12) };
            /* After countdown timer ticks to 0, change X direction */
            timer = random(24, 48);
            moveTo( scaled_position/8 );
            engine.insert(*this);
        }
    }

    void update() override
    {
        scaled_position += speed;
        timer--;
        if (0 == timer)
        {
            speed.x = random(-16, 16);
            timer = random(24, 48);
        }
        moveTo( scaled_position/8 );
        if (y() >= static_cast<int>(display.height()) )
        {
            destroy();
        }
        
    }

    void destroy(void)
    {
        getTiler().remove(*this);
    }    

private:
    NanoPoint scaled_position;
    NanoPoint speed;
    uint8_t timer;
};


class CShip: public CSprite
{
public:
    CShip()
    : CSprite({0, 0}, {8, 8}, shipImage) 
    { 
    }

    bool isAlive() { return hasTiler();  }

    void respawn()
    {
        if(!isAlive())
        {
            position = { random(display.width()), -8 };
            moveTo( position );
            engine.insert(*this);
        }
    }


    void update() override
    {
        CMPUData data;
        MPU_get(data);
        //range: (roll[-180,180]  pitch[-90,90]  yaw[-180,180])
        float scaleX=0.4f;
        float scaleY=0.8f;
        int dx=int(data.pitch*scaleX);
        int dy=int(data.roll*scaleY);
        //limit speed
        dx= MIN(5,MAX(-5,dx)); 
        dy= MIN(5,MAX(-5,dy));
        //move
        int nx = position.x - dx;
        int ny = position.y + dy;

        position.x=MIN( (int)display.width() -8, MAX(0,nx));
        position.y=MIN( (int)display.height()-8 ,MAX(0,ny));

        moveTo( position );
    }

private:
    NanoPoint position;
};

static const uint8_t maxCount = 2;

// These are our snow flakes
CAsteroid asteroids[maxCount];
CShip     ship;

bool onDraw()
{
    engine.getCanvas().clear();
    return true;
}

void setup_game()
{
    engine.setFrameRate( 10 );
    engine.begin();
    engine.drawCallback( onDraw );

    engine.getCanvas().setMode(CANVAS_MODE_TRANSPARENT);
    engine.refresh();
    //
    ship.respawn();
}

void addAsteroids()
{
    for (uint8_t i=0; i<maxCount; i++)
    {
        if (!asteroids[i].isAlive())
        {
            asteroids[i].bringToLife();
            break;
        }
    }
}



static uint8_t globalTimer=3;

void loop_game()
{
    if (!engine.nextFrame()) {
      return;
    }
    if (0 == (--globalTimer))
    {
        // Try to add new Asteroids every ~ 90ms 
        globalTimer = 3;
        addAsteroids();
    }
    engine.update();
    engine.display();
}

void setup_screen_saver(){
  engine.getCanvas().clear();
  engine.getCanvas().drawBitmap1(32, 0, 64, 64, rocket1);
  vTaskDelay(20);
  engine.getCanvas().drawBitmap1(32, 0, 64, 64, rocket2);
  vTaskDelay(20);
  engine.getCanvas().drawBitmap1(32, 0, 64, 64, rocket3);
  vTaskDelay(20);
  engine.getCanvas().drawBitmap1(32, 0, 64, 64, rocket4);
  vTaskDelay(20);
  engine.getCanvas().drawBitmap1(32, 0, 64, 64, rocket5);
  vTaskDelay(20);
  engine.getCanvas().drawBitmap1(32, 0, 64, 64, rocket6);
}

void oled_ship_task(void *pvParameters) 
{
  ESP_LOGD(__FUNCTION__, "Starting...");
  setup_screen_saver();  
  vTaskDelay(250);
  setup_game();

  while (1) {
    loop_game();
    vTaskDelay(5);
  }
  vTaskDelete(NULL); 
}
