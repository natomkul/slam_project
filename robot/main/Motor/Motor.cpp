#include "Motor.hpp"

Motor::Motor(const gpio_num_t portNumberA, const gpio_num_t portNumberB, Encoder &encoder, Pwm &pwm)
    : portNumberA(portNumberA), portNumberB(portNumberB), encoder(encoder), pwm(pwm)
{
    pwm.addChannel(portNumberA, 0);
    pwm.addChannel(portNumberB, 0);
}

Motor::~Motor() = default;

void Motor::moveForEncoderTicksCount(int ticks, PowerMode powerMode = slow)
{
    encoder.reset();
    Direction direction = ticks > 0 ? forward : reverse;

    if (direction == forward)
    {
        pwm[portNumberA].changeDuty(static_cast<uint8_t>(powerMode));
    }
    else
    {
        pwm[portNumberB].changeDuty(static_cast<uint8_t>(powerMode));
    }
    while (encoder.getPulseCount() < ticks)
        continue;

    if (direction == forward)
    {
        pwm[portNumberA].changeDuty(0);
    }
    else
    {
        pwm[portNumberB].changeDuty(0);
    }
}
