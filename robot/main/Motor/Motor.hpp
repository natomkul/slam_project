#pragma once

#include <cstdint>
#include <driver/gpio.h>
#include "Motor/Encoder.hpp"
#include "Pwm/Pwm.hpp"


/*
Class providing usage of single motor via PWM. Should calculate
ticks needed based on wheel diameter and allow to move by certain distance.

*/
class Motor {   
    public:
    Motor(const gpio_num_t portNumberA, const gpio_num_t portNumberB, const Encoder encoder, Pwm& pwm);
    ~Motor();
    
    private:
    const gpio_num_t portNumberA;
    const gpio_num_t portNumberB;
    const Encoder encoder;
    Pwm& pwm;
};
