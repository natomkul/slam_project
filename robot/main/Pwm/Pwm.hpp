#pragma once

#include "driver/ledc.h"
#include "driver/gpio.h"
#include <map>
#include "PwmChannel.hpp"

/*
Controls pwm and allows to add new channels.
*/
class Pwm
{
public:
    Pwm(uint32_t pwmFrequency)
        : pwmFrequency(pwmFrequency)
    {

        ledc_timer_config_t config = {
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .duty_resolution = LEDC_TIMER_8_BIT,
            .timer_num = LEDC_TIMER_0,
            .freq_hz = pwmFrequency,
            .clk_cfg = LEDC_USE_RC_FAST_CLK,
            .deconfigure = false,
        };
        esp_err_t esp_ret = ledc_timer_config(&config);
        if (esp_ret != ESP_OK)
        {
            printf("Error: Failed to initialize pwm, ERROR: 0x%x\r\n", esp_ret);
            abort();
        }
    };
    ~Pwm() = default;

    void addChannel(gpio_num_t portNumber, uint8_t duty);

    PwmChannel &operator[](gpio_num_t portNumber)
    {
        return pwmChannels.at(portNumber);
    }

private:
    uint32_t pwmFrequency;
    std::map<gpio_num_t, PwmChannel> pwmChannels;
};
