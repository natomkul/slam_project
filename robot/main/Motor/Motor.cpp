#include "Motor.hpp"

Motor::Motor(const gpio_num_t portNumberA, const gpio_num_t portNumberB, const Encoder encoder, Pwm& pwm)
    : portNumberA(portNumberA), portNumberB(portNumberB), encoder(encoder), pwm(pwm)
{
    pwm.addChannel(portNumberA, 0);
    pwm.addChannel(portNumberB, 0);
}

Motor::~Motor() = default;
