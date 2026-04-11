#pragma once

#include "driver/ledc.h"
#include <map>
#include "driver/gpio.h"
#include "PwmChannel.hpp"

class Pwm{
    public:
    Pwm(uint32_t frequency)
    : frequency(frequency){

        ledc_timer_config_t config = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .duty_resolution = LEDC_TIMER_8_BIT,
            .timer_num = LEDC_TIMER_0,
            .freq_hz = frequency,
            .clk_cfg = LEDC_USE_RC_FAST_CLK,
            .deconfigure = true,
        };
        ledc_timer_config(&config);
    };
    ~Pwm() = default;

    void addChannel(gpio_num_t portNumber, uint8_t duty, uint32_t frequency);
    
    PwmChannel& operator[](gpio_num_t portNumber){
        return pwmChannels.at(portNumber);
    }

    private:
        uint32_t frequency;
        std::map<gpio_num_t, PwmChannel> pwmChannels;
};
