#include "Accelerometer.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void Accelerometer::initializeI2c(){
    slave = i2c.createSlave(i2cAddress);
}

void Accelerometer::calibrate(){
    for(int i = 0; i < startupCommandsSize; i++){
        uint8_t currentReg[2] = {cmdRegister, startupCommands[i]};
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
    i2c.transmitReceive(slave, 0x04, buffer, 20);

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

Packet Accelerometer::receivePackedData()
{
    packedData[0] = 0x67;
    std::memcpy(packedData + 1, &data, sizeof(AccelerometerData));
    return Packet(packedData, 21);
}
