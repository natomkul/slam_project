#include "MotorManager.hpp"

namespace
{
    int64_t absTicks(int64_t ticks)
    {
        return ticks < 0 ? -ticks : ticks;
    }
}

void MotorManager::addMotor(const int motorNumber, Motor &motor)
{
    motors.insert({motorNumber, motor});
};

void MotorManager::moveMotorsForEncoderTicksCount(int64_t ticks, PowerMode powerMode)
{
    const int64_t targetTicks = absTicks(ticks);
    const Direction direction = ticks >= 0 ? forward : reverse;
    std::map<int, int64_t> startTicks;

    for (auto &[motorNumber, motor] : motors)
    {
        startTicks[motorNumber] = motor.getEncoderTicks();
        motor.start(direction == forward, powerMode);
    }

    while (true)
    {
        bool allReached = true;

        for (auto &[motorNumber, motor] : motors)
        {
            const int64_t movedTicks = motor.getEncoderTicks() - startTicks.at(motorNumber);
            if (absTicks(movedTicks) >= targetTicks)
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

void MotorManager::rotateMotorsForEncoderTicksCount(int64_t ticks, PowerMode powerMode)
{
    const int64_t targetTicks = absTicks(ticks);
    const Direction leftDirection = ticks >= 0 ? forward : reverse;
    const Direction rightDirection = ticks >= 0 ? reverse : forward;
    std::map<int, int64_t> startTicks;

    for (auto &[motorNumber, motor] : motors)
    {
        const Direction direction = motorNumber == 1 ? leftDirection : rightDirection;
        startTicks[motorNumber] = motor.getEncoderTicks();
        motor.start(direction == forward, powerMode);
    }

    while (true)
    {
        bool allReached = true;

        for (auto &[motorNumber, motor] : motors)
        {
            const int64_t movedTicks = motor.getEncoderTicks() - startTicks.at(motorNumber);
            if (absTicks(movedTicks) >= targetTicks)
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
