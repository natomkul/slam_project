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

    void addMotor(const int motorNumber, Motor &motor);
    void moveMotorsForEncoderTicksCount(int ticks, PowerMode PowerMode);
    void rotateMotorsForEncoderTicksCount(int ticks, PowerMode PowerMode);

private:
    std::map<int, Motor> motors;
};
