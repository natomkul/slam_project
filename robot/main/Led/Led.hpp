#pragma once

#include "driver/gpio.h"

#define HIGH 1
#define LOW 0
/*
Controls single LED
*/

class Led {
    public:
    Led(gpio_num_t portNumber, uint32_t blinkFrequency) :
    portNumber(portNumber),
    blinkFrequency(blinkFrequency)
    {
        gpio_config_t config;
        config.intr_type = GPIO_INTR_DISABLE;
        config.mode = GPIO_MODE_OUTPUT;
        config.pin_bit_mask = (1ULL << portNumber);
        config.pull_down_en = GPIO_PULLDOWN_DISABLE;
        config.pull_up_en = GPIO_PULLUP_DISABLE;
        gpio_config(&config);
    }
    ~Led()
    {
        off();
    }
    void on();
    void off();
    void blinkOn();
    void blinkOff();

    private:
    const gpio_num_t portNumber;
    const uint32_t blinkFrequency;
};
