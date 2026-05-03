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

int Accelerometer::receiveData(){
    i2c.transmitReceive(slave, dataRegister, receiveBuffer, 20);
    for(int i = 1; i < 21; i++){
        data[i] = receiveBuffer[i - 1];
    }
    return 21;
}

