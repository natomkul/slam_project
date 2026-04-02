#pragma once

#include <driver/i2c_master.h>
#include <atomic>
#include "I2c/I2c.hpp"

/*
Gets data from accelerometer and converts it into packets, 
which can be sent further. Packages are needed because this
is raw data afaik.
*/

struct AccelerometerData {
    int16_t magnitudeX;
    int16_t magnitudeY;
    int16_t magnitudeZ;
    int16_t rhall;
    int16_t gyroscopeX;
    int16_t gyroscopeY;
    int16_t gyroscopeZ;
    int16_t accelerationX;
    int16_t accelerationY;
    int16_t accelerationZ;
};

class Accelerometer {
    public:
    Accelerometer(I2c& i2c, const uint8_t i2cAddress, const uint8_t dataRegister, const uint8_t cmdRegister) :
    i2c(i2c),
    i2cAddress(i2cAddress),
    dataRegister(dataRegister),
    cmdRegister(cmdRegister)
    {
        initializeI2c();
    };
    ~Accelerometer() = default;

    void initializeI2c();
    void calibrate();
    void startMeasuring();
    void getMeasurement();
    AccelerometerData data;

    private:
    I2c& i2c;
    bool isMeasuring = false;
    std::shared_ptr<I2cSlave> slave;
    uint8_t buffer[20];
    const uint8_t startupCommands[3] = {0xB6, 0x11, 0x15}; 
    const uint8_t startupCommandsSize = 3;
    const uint8_t i2cAddress;
    const uint8_t dataRegister;
    const uint8_t cmdRegister;
};
