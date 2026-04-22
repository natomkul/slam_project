#include "Accelerometer.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace{
#define i2cAddress 0x68 
#define cmdRegister 0x7E
#define dataRegister 0x04
}

void Accelerometer::initializeI2c(){
    slave = i2c.createSlave(i2cAddress);
}

void Accelerometer::calibrate(){
    constexpr uint8_t calibrateCommands[3] = {0xB6, 0x11, 0x15}; 
    constexpr uint8_t calibrateCommandsSize = 3;

    for(int i = 0; i < calibrateCommandsSize; i++){
        uint8_t currentReg[2] = {cmdRegister, calibrateCommands[i]};
        i2c.transmit(slave, cmdRegister, currentReg, 2);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void Accelerometer::startMeasuring(){
    calibrate();
    isMeasuring = true;
    while(isMeasuring){
        getMeasurement();
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void Accelerometer::getMeasurement(){
    i2c.transmitReceive(slave, dataRegister, buffer, 20);

    data.magnitudeX = (buffer[0] << 8) | buffer[1];
    data.magnitudeY = (buffer[2] << 8) | buffer[3];
    data.magnitudeZ = (buffer[4] << 8) | buffer[5];
    data.rhall = (buffer[6] << 8) | buffer[7];
    data.gyroscopeX = (buffer[8] << 8) | buffer[9];
    data.gyroscopeY = (buffer[10] << 8) | buffer[11];
    data.gyroscopeZ = (buffer[12] << 8) | buffer[13];
    data.accelerationX = (buffer[14] << 8) | buffer[15];
    data.accelerationY = (buffer[16] << 8) | buffer[17];
    data.accelerationZ = (buffer[18] << 8) | buffer[19];
}
