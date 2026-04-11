#pragma once

#include <cstdint>
#include "driver/ledc.h"
#include "driver/gpio.h"

class PwmChannel{
    public:
    PwmChannel(gpio_num_t portNumber, uint8_t duty, uint32_t frequency){
        ledc_channel_config_t config = {
            .gpio_num = portNumber,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = LEDC_CHANNEL_0,
            .intr_type = LEDC_INTR_DISABLE,
            .timer_sel = LEDC_TIMER_0,
            .duty = duty,
            .hpoint = 0,
            .sleep_mode = LEDC_SLEEP_MODE_KEEP_ALIVE,
            .flags = {
                .output_invert = false,
            },
            .deconfigure = true,
        };
        ledc_channel_config(&config);
    }
    ~PwmChannel() = default;

    void changeDuty();
    void changeFrequency();

    private:
        uint8_t duty;
        uint32_t frequency;
};
