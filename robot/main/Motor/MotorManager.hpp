#pragma once

#include <cstdint>
#include <map>
#include "driver/gpio.h"
#include "Motor/Motor.hpp"
#include "Motor/Encoder.hpp"

class MotorManager
{
public:
    Motor operator[](const int motorNumber)
    {
        return motors.at(motorNumber);
    }

    void addMotor(const int motorNumber, Motor &motor);
    void moveMotorsForEncoderTicksCount(int64_t ticks, PowerMode powerMode);
    void rotateMotorsForEncoderTicksCount(int64_t ticks, PowerMode powerMode);

private:
    std::map<int, Motor> motors;
};
