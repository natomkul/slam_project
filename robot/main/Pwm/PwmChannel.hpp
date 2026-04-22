#pragma once

#include <cstdint>
#include "driver/ledc.h"
#include "driver/gpio.h"

class PwmChannel{
    public:
    PwmChannel(gpio_num_t portNumber, uint8_t duty){
        channel = reserveNextChannel();
        ledc_channel_config_t config = {
            .gpio_num = portNumber,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = channel,
            .intr_type = LEDC_INTR_DISABLE,
            .timer_sel = LEDC_TIMER_0,
            .duty = duty,
            .hpoint = 0,
            .sleep_mode = LEDC_SLEEP_MODE_KEEP_ALIVE,
            .flags = {
                .output_invert = false,
            },
            .deconfigure = false,
        };
        esp_err_t esp_ret = ledc_channel_config(&config);
        if(esp_ret != ESP_OK){
            printf("Error: Failed to initialize pwm, ERROR: 0x%x\r\n", esp_ret);
            abort();
        }
    }
    ~PwmChannel() = default;

    void changeDuty(uint8_t duty);
    void changeFrequency(uint32_t frequency);

    ledc_channel_t reserveNextChannel();

    private:
        ledc_channel_t channel;
        uint8_t duty;
        uint32_t frequency;
};
