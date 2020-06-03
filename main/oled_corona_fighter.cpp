#include "app.h"


#ifdef BADGE_CORONA


#define TAG "OLED Ship"

#define GAME_X1 0
#define GAME_X2 125
#define GAME_HEIGHT (GAME_X2-GAME_X1+1)
#define GAME_Y1 0
#define GAME_Y2 61
#define GAME_WIDTH (GAME_Y2-GAME_Y1+1)

static int game_total_score=0;
static int game_level_score=0;
const  int game_level_next=10;
static int game_level=0;
const  int game_level_max=10;
const  int game_total_score_max=game_level_next*game_level_max;

#ifndef min
#define min(x,y) ((x)<(y)?(x):(y))
#endif

#ifndef max
#define max(x,y) ((x)>(y)?(x):(y))
#endif

const  int game_lives_max=9;
static int game_lives=game_lives_max;

static bool  game_active=false;
const  int CORONAS_MAX=game_level_max+2;

void game_score_inc()
{
    game_level_score++;
    game_total_score++;
    if(game_level_score>=game_level_next) {
        game_level_score=0;
        game_level++;
    }
}

void game_score_dec()
{
    game_level_score--;
    game_total_score--;
    if(game_level_score<=0) {
        game_level_score=0;
        game_level++;
    }
}

//https://github.com/lexus2k/ssd1306/tree/master/examples/direct_draw/move_sprite

/*
 * Define sprite width. The width can be of any size.
 * But sprite height is always assumed to be 8 pixels
 * (number of bits in single byte).
 */
#define SCALE 8


//NanoEngine1 engine;
//typedef NanoFixedSprite<NanoEngine1, engine> CSprite;
//class CCorona: public CSprite


typedef NanoEngine1<DISPLAY_TYPE> NanoEngineCorona;
typedef NanoFixedSprite<NanoEngineCorona::TilerT> CSprite;
NanoEngineCorona engine(display);

class CCorona: public CSprite
{
public:
    CCorona()
    : CSprite({0, 0}, {8, 8}, nullptr) 
    { 
    }

    bool isAlive() { return hasTiler(); }

    void generate()
    {
        if(!isAlive())
        {
            int idx = random( sizeof(CoronaImage)/sizeof(CoronaImage[0]) );
            setBitmap( CoronaImage[idx] );
            /* Set initial position in scaled coordinates */
            scaled_position = { GAME_X1+random(GAME_WIDTH*SCALE), -8*SCALE};
            /* Use some random speed */
            speed = { random(-2*SCALE, 2*SCALE), random( SCALE/2, (SCALE*3)/4) };
            /* After countdown timer ticks to 0, change X direction */
            timer = random(24, 48);
            moveTo( scaled_position/SCALE );
            engine.insert(*this);
        }

    }

    void update() override
    {
        scaled_position += speed;
       
        timer--;
        if (0 == timer)
        {
            /* Change movement direction */
            speed.x = random(-2*SCALE, 2*SCALE);
            timer = random(24, 48);
        }
        moveTo( scaled_position/SCALE );
        if ( y() >= GAME_Y2 )
        {
            destroyed();
        }
    }

    void destroyed(void)
    {
        getTiler().remove(*this);
    }

private:
    NanoPoint scaled_position;
    NanoPoint speed;
    uint8_t timer;
};


class CFighter: public CSprite
{
public:
    CFighter()
    : CSprite({0, 0}, {8, 8}, fighterImage) 
    { 
    }

    bool isAlive() { return hasTiler(); }

    void respawn()
    {
        if(!isAlive())
        {
            position = { GAME_X1+random(GAME_WIDTH), GAME_Y2-8 };
            moveTo( position );
            engine.insert(*this);
        }
    }

    void die(void)
    {
        getTiler().remove(*this);
    }
    
    void update() override
    {
        CMPUData data;
        MPU_get(data);
        //range: (roll[-180,180]  pitch[-90,90]  yaw[-180,180])
        float scaleX=0.4f;
        int dx=int(data.pitch*scaleX);
        //limit speed
        dx= MIN(5,MAX(-5,dx)); 
        //move
        int nx = position.x - dx;

        position.x=MIN( GAME_X2-8, MAX(GAME_X1,nx));

        moveTo( position );
    }

private:
    NanoPoint position;
};


class CBullet: public CSprite
{
public:
    CBullet()
    : CSprite({0, 0}, {8, 8}, bulletImage) 
    { 
    }

    bool isAlive() { return hasTiler(); }

    void fire(int x, int y)
    {
        if(!isAlive())
        {
            scaled_position = { x*SCALE, y*SCALE };
            speed = { 0, -(3*SCALE)/2 };
            moveTo( scaled_position/SCALE );
            engine.insert(*this);
        }
    }

    void destroyed(void)
    {
        getTiler().remove(*this);
    }

    void update() override
    {
        scaled_position += speed;
        moveTo( scaled_position/SCALE );
        if (y() >= display.height() || y() <= 0 )
        {
            destroyed();
        }
    }

private:
    NanoPoint scaled_position;
    NanoPoint speed;
};


bool is_rects_intersects(int x1, int y1, int x2, int y2, 
                         int x3, int y3, int x4, int y4) 
{ 
    // gives bottom-left point 
    // of intersection rectangle 
    int x5 = max(x1, x3); 
    int y5 = max(y1, y3); 
  
    // gives top-right point 
    // of intersection rectangle 
    int x6 = min(x2, x4); 
    int y6 = min(y2, y4); 
  
    // no intersection 
    return ! (x5 > x6 || y5 > y6);
} 


#define MAX_BULLETS 10

// These are our snow flakes
CCorona corona[CORONAS_MAX];
CFighter fighter;
CBullet bullet[MAX_BULLETS];

bool onDraw()
{
    engine.getCanvas().clear();

    //show score
    if(game_lives)
        engine.getCanvas().drawVLine(0,  63-(game_lives*64)/game_lives_max,63);

    if(game_total_score)
        engine.getCanvas().drawVLine(127,63-(game_total_score*64)/game_total_score_max,63);

    return true;
}

void setup_game()
{
    game_active=true;
    engine.setFrameRate( 10 );
    engine.begin();
    engine.drawCallback( onDraw );

    engine.getCanvas().setMode(CANVAS_MODE_TRANSPARENT);
    engine.refresh();
    //
    fighter.respawn();
}

void addCorona()
{
    int MC = MIN(CORONAS_MAX, game_level+2);
    for (uint8_t i=0; i<MC; i++)
    {
        if (!corona[i].isAlive())
        {
            corona[i].generate();
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
        addCorona();
    }
    //check kills
    for(int i=0; i<MAX_BULLETS;i++)
        if(bullet[i].isAlive())
        {
            int bx1=bullet[i].x()+4;
            int bx2=bullet[i].x()+5;
            int by1=bullet[i].y();
            int by2=bullet[i].y()+4;
            for(int j=0; j<CORONAS_MAX;j++)
                if(corona[j].isAlive()) 
                {
                    int cx1=corona[j].x();
                    int cy1=corona[j].y();
                    int cx2=cx1+7;
                    int cy2=cy1+7;                    
                    if( is_rects_intersects( bx1, by1, bx2, by2, cx1, cy1, cx2, cy2))
                    {
                        bullet[i].destroyed();
                        corona[j].destroyed();
                        leds_alarm_set(true, LEDS_STRIP_GREEN, 0.5f, 10, false);
                        game_score_inc();
                        break;
                    }
                }
        }
    //check fighter
    int fx1=fighter.x();
    int fy1=fighter.y();
    int fx2=fx1+7;
    int fy2=fy1+7;
    for(int j=0; j<CORONAS_MAX;j++)
        if(corona[j].isAlive()) 
        {
            int cx1=corona[j].x();
            int cy1=corona[j].y();
            int cx2=cx1+7;
            int cy2=cy1+7;
            if(is_rects_intersects( fx1, fy1, fx2, fy2, cx1, cy1, cx2, cy2))
            {
                leds_alarm_set(true, LEDS_STRIP_RED, 0.6f, 10, false);
                corona[j].destroyed();
                game_score_inc();
                game_lives--;
                if(game_lives<=0) {
                    fighter.die();
                    game_active=false;
                }
                break;
            }
         }
    //fire
    int max_y=0;
    for(int i=0; i<MAX_BULLETS; i++)
        if( bullet[i].isAlive() )
            max_y=MAX(max_y, bullet[i].y());
    //printf("max_y=%i fy=%i\n",max_y,fighter.y());
    if(fighter.y()-max_y > 10) {
        for(int i=0; i<MAX_BULLETS; i++)
            if( !bullet[i].isAlive() )
            {
                bullet[i].fire( fighter.x(), fighter.y() );
                break;
            }
    }

    engine.update();//moves
    engine.display();
}

void setup_screen_saver_start()
{
    display.clear();
    display.drawBitmap1(0, 0, 128, 64, fighter_stop_corona);
    vTaskDelay(200);
    display.drawBitmap1(0, 0, 128, 64, fighter_masks);
    vTaskDelay(100);
    display.drawBitmap1(0, 0, 128, 64, fighter_stay_at_home);
    vTaskDelay(100);
    display.drawBitmap1(0, 0, 128, 64, fighter_or);
    vTaskDelay(100);
    display.drawBitmap1(0, 0, 128, 64, fighter_covid_die);
    vTaskDelay(100);
}

void setup_screen_saver_alive(){
  display.clear();
  display.drawBitmap1(0, 0, 128, 64, fighter_stop_corona);
  vTaskDelay(200);
  display.drawBitmap1(0, 0, 128, 64, fighter_masks);
  vTaskDelay(100);
  display.drawBitmap1(0, 0, 128, 64, fighter_covid_cancel);
  vTaskDelay(20);
}

void setup_screen_saver_die(){
  display.clear();
  display.drawBitmap1(0, 0, 128, 64, fighter_stop_corona);
  vTaskDelay(200);
  display.drawBitmap1(0, 0, 128, 64, fighter_stay_at_home);
  vTaskDelay(100);
  display.drawBitmap1(0, 0, 128, 64, fighter_or);
  vTaskDelay(100);
  display.drawBitmap1(0, 0, 128, 64, fighter_covid_die);
  vTaskDelay(20);
}

void oled_corona_fighter_task(void *pvParameters) 
{
  ESP_LOGD(__FUNCTION__, "Starting...");
  setup_screen_saver_start();  
  vTaskDelay(250);
  setup_game();

  while (game_active) {
    loop_game();
    vTaskDelay(5);
  }
 
  if(fighter.isAlive())
    setup_screen_saver_alive();  
  else 
    setup_screen_saver_die();  

  vTaskDelete(NULL); 
}

#endif