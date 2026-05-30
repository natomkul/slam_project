#pragma once

#include <map>
#include "driver/gpio.h"
#include "Motor/Motor.hpp"
#include "Motor/Encoder.hpp"

class MotorManager
{
public:
    MotorManager() = default;
    ~MotorManager() = default;

    Motor operator[](gpio_num_t portNumber)
    {
        return motors.at(portNumber);
    }

    void addMotor(const gpio_num_t portNumberA, const gpio_num_t portNumberB, const Encoder encoder);

private:
    std::map<gpio_num_t, Motor> motors;
};
