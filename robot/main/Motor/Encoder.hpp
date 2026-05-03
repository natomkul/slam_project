#pragma once

#include <cstdint>

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
    Encoder(const uint16_t ppr, const gpio_num_t channelA, const gpio_num_t channelB):
    ppr(ppr), channelA(channelA),channelB(channelB)
    {

    }
    ~Encoder(){

    };

    private:
    const uint16_t ppr; // pulses per revolution
    const gpio_num_t channelA;
    const gpio_num_t channelB;
};


