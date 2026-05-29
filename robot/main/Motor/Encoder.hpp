#pragma once

#include <cstdint>
#include <esp_timer.h>

enum Event {
    clockwise,
    counterclockwise
};

// 7ppr
/*
Class having two channels connected to encoders. Store events to some buffer,
that will be further send to computer.
*/
class Encoder{
    public:
    Encoder(const uint16_t ppr, const gpio_num_t channelA, const gpio_num_t channelB, float wheelRadius):
    ppr(ppr), channelA(channelA), channelB(channelB), wheelRadius(wheelRadius)
    {
        gpio_config_t io_conf{};
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pin_bit_mask = (1ULL << channelA) | (1ULL << channelB);
        io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        gpio_config(&io_conf);

        previousState = (gpio_get_level(channelA) << 1) | gpio_get_level(channelB);
        data[0] = 0x68;
    }
    ~Encoder() = default;

    void update();

    float getRevolutions() const;
    float getDistanceInMeters() const;
    float getVelocityInMpS();
    void reset();

    int receiveData();

    private:
    const uint16_t ppr; // pulses per revolution
    const gpio_num_t channelA;
    const gpio_num_t channelB;
    const float wheelRadius;

    uint8_t data[4];

    int16_t pulseCount{0};
    uint8_t previousState{0};
    Event lastEvent{clockwise};
};


