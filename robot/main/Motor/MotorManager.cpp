#include "MotorManager.hpp"
#include <cmath>

MotorManager::MotorManager() = default;

MotorManager::~MotorManager() = default;

void MotorManager::addMotor(const int motorNumber, Motor &motor)
{
    motors.insert({motorNumber, motor});
};

void MotorManager::moveMotorsForEncoderTicksCount(int ticks, PowerMode powerMode)
{
    const int targetTicks = std::abs(ticks);
    const Direction direction = ticks >= 0 ? forward : reverse;

    for (auto &[_, motor] : motors)
    {
        motor.resetEncoder();
        motor.start(direction == forward, powerMode);
    }

    while (true)
    {
        bool allReached = true;

        for (auto &[_, motor] : motors)
        {
            if (std::abs(motor.getEncoderTicks()) >= targetTicks)
            {
                motor.stop();
            }
            else
            {
                allReached = false;
            }
        }

        if (allReached)
        {
            break;
        }
    }
}

void MotorManager::rotateMotorsForEncoderTicksCount(int ticks, PowerMode powerMode)
{
    const int targetTicks = std::abs(ticks);
    const Direction leftDirection = ticks >= 0 ? forward : reverse;
    const Direction rightDirection = ticks >= 0 ? reverse : forward;

    for (auto &[motorNumber, motor] : motors)
    {
        const Direction direction = motorNumber == 1 ? leftDirection : rightDirection;
        motor.resetEncoder();
        motor.start(direction == forward, powerMode);
    }

    while (true)
    {
        bool allReached = true;

        for (auto &[_, motor] : motors)
        {
            if (std::abs(motor.getEncoderTicks()) >= targetTicks)
            {
                motor.stop();
            }
            else
            {
                allReached = false;
            }
        }

        if (allReached)
        {
            break;
        }
    }
}
