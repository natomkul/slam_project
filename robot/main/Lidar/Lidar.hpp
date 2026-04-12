#pragma once

#include "LidarFrame.hpp"
#include "Uart/Uart.hpp"
#include "DataExchanger/Packet.hpp"
#include <cstdint>
#include <chrono>

/*
Class to get data from LiDAR sensor, and convert it
into packages that can be sent further.
*/

// The frames are already constructed so do we really
// need this structs or just blindly transmit them to PC?

class Lidar {
    public:
    Uart uart;
    const uint16_t bufferSize = 2048;
    uint8_t data[2048];
    uint8_t size;
    int length;

    Lidar(uint16_t TxPin, uint16_t RxPin, int baudRate) : uart{TxPin, RxPin, baudRate, data, 2048, length}{};
    ~Lidar(){
        isReceiving = false;
    };
        
    Packet receiveData();
    void stopReceiving();

    private:
    bool isReceiving = false;
    uint sleepTime = 10;

};
