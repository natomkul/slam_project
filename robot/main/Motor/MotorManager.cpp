#include "MotorManager.hpp"

MotorManager::MotorManager() = default;

MotorManager::~MotorManager() = default;

void MotorManager::addMotor(const int motorNumber, Motor &motor)
{
    motors.insert({motorNumber, motor});
};
