#include "MotorManager.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace
{
    constexpr int rampSteps = 20;
    constexpr TickType_t rampStepDelay = pdMS_TO_TICKS(50);

    int64_t absTicks(int64_t ticks)
    {
        return ticks < 0 ? -ticks : ticks;
    }

    void rampMotors(std::map<int, Motor> &motors, const std::map<int, bool> &directions, PowerMode powerMode)
    {
        const int targetDuty = static_cast<int>(powerMode);

        for (int step = 1; step <= rampSteps; step++)
        {
            const PowerMode duty = static_cast<PowerMode>(targetDuty * step / rampSteps);

            for (auto &[motorNumber, motor] : motors)
            {
                motor.start(directions.at(motorNumber), duty);
            }

            vTaskDelay(rampStepDelay);
        }
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
    std::map<int, bool> directions;

    for (auto &[motorNumber, motor] : motors)
    {
        startTicks[motorNumber] = motor.getEncoderTicks();
        directions[motorNumber] = direction == forward;
    }

    rampMotors(motors, directions, powerMode);

    while (true)
    {
        bool allReached = true;

        for (auto &[motorNumber, motor] : motors)
        {
            const int64_t movedTicks = motor.getEncoderTicks() - startTicks.at(motorNumber);
            if (absTicks(movedTicks) >= targetTicks * motor.encoder.tickCorrection)
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
    std::map<int, bool> directions;

    for (auto &[motorNumber, motor] : motors)
    {
        const Direction direction = motorNumber == 1 ? leftDirection : rightDirection;
        startTicks[motorNumber] = motor.getEncoderTicks();
        directions[motorNumber] = direction == forward;
    }

    rampMotors(motors, directions, powerMode);

    while (true)
    {
        bool allReached = true;

        for (auto &[motorNumber, motor] : motors)
        {
            const int64_t movedTicks = motor.getEncoderTicks() - startTicks.at(motorNumber);
            if (absTicks(movedTicks) >= targetTicks * motor.encoder.tickCorrection)
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
