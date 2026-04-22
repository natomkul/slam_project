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
    Accelerometer(I2c& i2c) : i2c(i2c)
    {
        initializeI2c();
    };
    ~Accelerometer()
    {
        isMeasuring = false;
    };

    AccelerometerData data;

    void startMeasuring();
    void getMeasurement();

    private:
    I2c& i2c;
    bool isMeasuring = false;
    uint8_t buffer[20];
    std::shared_ptr<I2cSlave> slave;

    void initializeI2c();
    void calibrate();
};
