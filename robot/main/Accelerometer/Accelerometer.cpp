#include "Accelerometer.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace
{
#define i2cAddress 0x69
#define cmdRegister 0x7E
#define dataRegister 0x04
}

Accelerometer::Accelerometer(I2c &i2c) : i2c(i2c)
{
    data[0] = 0x67;
    initializeI2c();
}

Accelerometer::~Accelerometer()
{
    isMeasuring = false;
}

void Accelerometer::initializeI2c()
{
    slave = i2c.createSlave(i2cAddress);
    calibrate();
}

void Accelerometer::calibrate()
{
    constexpr uint8_t calibrateCommands[3] = {0xB6, 0x11, 0x15};
    constexpr uint8_t calibrateCommandsSize = 3;

    for (int i = 0; i < calibrateCommandsSize; i++)
    {
        uint8_t currentReg[2] = {cmdRegister, calibrateCommands[i]};
        i2c.transmit(slave, cmdRegister, currentReg, 2);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

int Accelerometer::receiveData()
{
    auto currentTimestamp = std::chrono::steady_clock::now();
    const std::chrono::duration elapsed{currentTimestamp - previousTimestamp};
    previousTimestamp = currentTimestamp;

    i2c.transmitReceive(slave, dataRegister, receiveBuffer, 20);
    for (int i = 1; i < 21; i++)
    {
        data[i] = receiveBuffer[i - 1];
    }
    uint64_t nano = elapsed.count();

    for (int i = 21; i < 21 + 8; i++)
    {
        data[i] = nano & 0xFF;
        nano >>= 8;
    }
    return 29;
}
