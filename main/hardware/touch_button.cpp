#include "app.h"

static bool touchpad_enabled = false;

static const char *TAG = __FILE__;

typedef struct  {
  bool           is_enabled = {false};
  TOUCHPAD_STATE tp_last_state = {TOUCHPAD_STATE_OFF};
  //run-time values
  uint16_t       touch_filter_value = {0};
  int            duration  = {0};
  uint64_t       last_click_time = {0};
  bool           last_click_was  = {false};
  int            last_click_duration={0};
  uint64_t       last_long_time = {0};
  bool           last_long_was  = {false};
} TOUCH_BUTTON;

static TOUCH_BUTTON _touch_buttons[TOUCH_PAD_MAX];


//TOUCHPAD_EVENT_HANDLER event_handlers[TOUCHPAD_EVENTS];


// https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/touch_pad.html
void touchpad_task_init() 
{
    static bool bInited=false;
    if(bInited) 
      return;
    
    //memset(event_handlers, 0, sizeof(event_handlers));
    // Initialize touch pad peripheral.
    // The default fsm mode is software trigger mode.
    touch_pad_init();
    // Set reference voltage for charging/discharging
    // In this case, the high reference valtage will be 2.7V - 1V = 1.7V
    // The low reference voltage will be 0.5
    // The larger the range, the larger the pulse count value.
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    
    #ifdef TOUCHPAD_FILTER_MODE
    touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);
    #endif

    bInited=true;
  }


// void touchpad_add_handler(TOUCHPAD_EVENT eventId, TOUCHPAD_EVENT_HANDLER handler)
// {
//   if(!touchpad_enabled) 
//     return;
//   event_handlers[eventId] = handler;
// }
esp_err_t touchpad_proces(touch_pad_t pin) 
{
    esp_err_t res=ESP_OK;
    if(!_touch_buttons[pin].is_enabled)
      return ESP_ERR_INVALID_ARG;

    uint16_t& touch_filter_value  = _touch_buttons[pin].touch_filter_value;
    int&      duration            = _touch_buttons[pin].duration;
    uint64_t& last_click_time     = _touch_buttons[pin].last_click_time;
    bool&     last_click_was      = _touch_buttons[pin].last_click_was;
    int&      last_click_duration = _touch_buttons[pin].last_click_duration;
    uint64_t& last_long_time      = _touch_buttons[pin].last_long_time;
    bool&     last_long_was       = _touch_buttons[pin].last_long_was;
    TOUCHPAD_STATE& tp_last_state = _touch_buttons[pin].tp_last_state;


    res=touch_pad_read_filtered(pin, &touch_filter_value);
    if(res!=ESP_OK){
      ESP_LOGE(TAG, "%s\n", esp_err_to_name(res));
      return res;
    }
    TOUCHPAD_STATE state = touch_filter_value<=TOUCHPAD_TRESH? TOUCHPAD_STATE_ON : TOUCHPAD_STATE_OFF;
    duration++;
    uint32_t curtime = millis(); //overflow?
    if(state!=tp_last_state)
    {
      //UP / DOWN
      TOUCHPAD_EVENT event = state==TOUCHPAD_STATE_ON ? TOUCHPAD_EVENT_DOWN : TOUCHPAD_EVENT_UP;
      //if(event_handlers[event])
      //  event_handlers[event](event, duration);
      //ESP_LOGD(TAG,"Touchpad event: %i", event);


      //dblclick detection
      if(last_click_was && event==TOUCHPAD_EVENT_DOWN && curtime-last_click_time<TOUCHPAD_DBLCLICK_DELAY){
        //if(event_handlers[TOUCHPAD_EVENT_DBLCLICK])
        //  event_handlers[TOUCHPAD_EVENT_DBLCLICK](TOUCHPAD_EVENT_DBLCLICK, duration);
        last_click_was=false;
        //ESP_LOGD(TAG,"Touchpad event: DBLCLICK");
      } 
      else //delayed click i.e. two separates clicks detection
      {
        if(last_click_was && curtime-last_click_time>=TOUCHPAD_DBLCLICK_DELAY){
          //if(event_handlers[TOUCHPAD_EVENT_CLICK])
          //  event_handlers[TOUCHPAD_EVENT_CLICK](TOUCHPAD_EVENT_CLICK, last_click_duration);
          last_click_was=false;
          //ESP_LOGD(TAG,"Touchpad event: CLICK delayed 1");
        }
        ////
        if(event==TOUCHPAD_EVENT_DOWN){
          last_click_time = curtime;
          last_click_was  = true;
          last_click_duration=duration;        
        }
      }

      //longpress detection start
      if(event==TOUCHPAD_EVENT_DOWN){
        last_long_was  = true;
        last_long_time = curtime;
      }
      /////////////////////////
      duration=0;
      tp_last_state=state;
      //ESP_LOGD(TAG, "event=%i dur=%i\n", event, duration);
    }  
    else  // no state changed - long press detection, delayed click
    {
        if(last_click_was && curtime-last_click_time>=TOUCHPAD_DBLCLICK_DELAY){
          // if(event_handlers[TOUCHPAD_EVENT_CLICK]) {
          //   event_handlers[TOUCHPAD_EVENT_CLICK](TOUCHPAD_EVENT_CLICK, last_click_duration);
          //   //ESP_LOGD(TAG,"Touchpad event: CLICK delayed 2, event handler call");
          // }
          last_click_was=false;
          //ESP_LOGD(TAG,"Touchpad event: CLICK delayed 2");
        }
        if(state==TOUCHPAD_STATE_ON && last_long_was && curtime-last_long_time>=TOUCHPAD_LONGPRESS_DELAY){
          // if(event_handlers[TOUCHPAD_EVENT_LONGPRESS])
          //   event_handlers[TOUCHPAD_EVENT_LONGPRESS](TOUCHPAD_EVENT_LONGPRESS, curtime-last_long_time);
          last_long_time=curtime; 
          //ESP_LOGD(TAG,"Touchpad event: LONG PRESS");
        }
    }
  return ESP_OK;
}

void touchpad_task(void *pvParameters) 
{
  ESP_LOGD(TAG, "%s: Starting ", __FUNCTION__);
  if(!touchpad_enabled) {
    vTaskDelete(NULL);
    return;
  }

  LOG_FUNC_BEGIN(TAG)
  while (1) 
  {
    for(int i=TOUCH_PAD_NUM0; i<TOUCH_PAD_MAX; i++)
    {
      touch_pad_t pin=static_cast<touch_pad_t>(i);
      if( _touch_buttons[pin].is_enabled)
        touchpad_proces(pin);
    }
    //ESP_LOGD(TAG, "touch_filter_value=%i\n", touch_filter_value);
    vTaskDelay(5);
  }
  LOG_FUNC_END(TAG)
  vTaskDelete(NULL);
}

CTouchButton::CTouchButton(touch_pad_t channel)
    : m_channel(channel),
      m_enabled(true)
{

}

void CTouchButton::init()
{
  touchpad_task_init();
  //for (int i = 0;i< TOUCH_PAD_MAX;i++) {
  //    touch_pad_config(i, TOUCH_THRESH_NO_USE);
  //}
  touch_pad_config(m_channel, TOUCHPAD_THRESH_NO_USE);
  _touch_buttons[m_channel].tp_last_state=TOUCHPAD_STATE_OFF;
  _touch_buttons[m_channel].is_enabled=true;
  touchpad_enabled=true;
}

TOUCHPAD_STATE CTouchButton::get_state(void)
{
  if(!m_enabled) 
    return TOUCHPAD_STATE_DISABLED;
  return _touch_buttons[m_channel].tp_last_state;
}

void CTouchButton::run()
{
  ESP_LOGD(TAG, "%s", __FUNCTION__);
  static bool isRun=false;
  if(isRun) 
    return;
  xTaskCreate(&touchpad_task, "Touchpad", 1024 * 4, NULL, 5, NULL);
  isRun=true;
}