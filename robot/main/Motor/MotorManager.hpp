#pragma once

#include <map>
#include "driver/gpio.h"
#include "Motor/Motor.hpp"
#include "Motor/Encoder.hpp"

class MotorManager
{
public:
    MotorManager();
    ~MotorManager();

    Motor operator[](const int motorNumber)
    {
        return motors.at(motorNumber);
    }

    void addMotor(const int motorNumber, Motor motor);

private:
    std::map<int, Motor> motors;
};
