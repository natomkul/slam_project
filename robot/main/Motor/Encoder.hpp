#pragma once

#include <cstdint>
#include <driver/gpio.h>
#include <esp_timer.h>
#include <atomic>

enum Event
{
    clockwise,
    counterclockwise
};

/*
Class having two channels connected to encoders. Store events to some buffer,
that will be further send to computer.
*/
class Encoder
{
public:
    Encoder(const uint16_t ppr, const gpio_num_t channelA, const gpio_num_t channelB, const float wheelRadius);
    ~Encoder();

    void update();

    float getRevolutions() const;
    float getDistanceInMeters() const;
    float getVelocityInMpS();
    int16_t getPulseCount();
    void reset();

    int receiveData();

private:
    const uint16_t ppr; // pulses per revolution
    const gpio_num_t channelA;
    const gpio_num_t channelB;
    const float wheelRadius;

    uint8_t data[4];

    std::atomic<int16_t> pulseCount{0};
    uint8_t previousState{0};
    Event lastEvent{clockwise};
};
