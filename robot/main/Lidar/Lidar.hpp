#pragma once

#include "LidarFrame.hpp"
#include "Uart/Uart.hpp"

/*
Class to get data from LiDAR sensor, and convert it
into packages that can be sent further.
*/

// The frames are already constructed so do we really
// need this structs or just blindly transmit them to PC?

class Lidar {
    public:
    Lidar(uint32_t reg, uint32_t baudRate) : uart{reg, baudRate}{};
    ~Lidar() = default;

    void startReceiving();
    void stopReceiving();

    private:
    Uart uart;
};
