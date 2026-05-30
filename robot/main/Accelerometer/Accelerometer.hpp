#pragma once

#include <driver/i2c_master.h>
#include <atomic>
#include "I2c/I2c.hpp"

/*
Gets data from accelerometer and converts it into packets,
which can be sent further. Packages are needed because this
is raw data afaik.
*/

// struct data {
//     uint8_t header{0x67};
//     int16_t magnitudeX;
//     int16_t magnitudeY;
//     int16_t magnitudeZ;
//     int16_t rhall;
//     int16_t gyroscopeX;
//     int16_t gyroscopeY;
//     int16_t gyroscopeZ;
//     int16_t accelerationX;
//     int16_t accelerationY;
//     int16_t accelerationZ;
// };
class Accelerometer
{
public:
    Accelerometer(I2c &i2c) : i2c(i2c)
    {
        data[0] = 0x67;
        initializeI2c();
    };
    ~Accelerometer()
    {
        isMeasuring = false;
    };

    uint8_t data[21];

    int receiveData();

private:
    I2c &i2c;
    bool isMeasuring = false;
    uint8_t receiveBuffer[20];
    std::shared_ptr<I2cSlave> slave;

    void initializeI2c();
    void calibrate();
};
