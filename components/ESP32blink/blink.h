#pragma once

class CBlink {
    bool        m_inited = {false};
    gpio_num_t  m_pin    = {GPIO_NUM_NC};
  public: 
    CBlink(gpio_num_t pin) 
     : m_pin(pin)
    {

    }

    ~CBlink() {}

    void init()
    {
        if(!m_inited && m_pin!=GPIO_NUM_NC)
        {
            gpio_config_t io_conf;
            // disable interrupt
            io_conf.intr_type = GPIO_INTR_DISABLE;
            // set as output mode
            io_conf.mode = GPIO_MODE_OUTPUT;
            // bit mask of the pins that you want to set,e.g.GPIO18/19
            io_conf.pin_bit_mask = (1ULL << m_pin);
            // disable pull-down mode
            io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
            // disable pull-up mode
            io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
            // configure GPIO with the given settings
            gpio_config(&io_conf);
            m_inited=true;
        }
    }

    void on(){
        if(m_inited)
            gpio_set_level(m_pin, 1); //OFF=1
    }

    void off(){
        if(m_inited)
            gpio_set_level(m_pin, 0); //ON=1
    }
};