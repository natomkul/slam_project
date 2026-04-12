#include "Packet.hpp"

Packet::Packet(DataType type, uint16_t length, const uint8_t* data) : type(type), length(length)
{
    if (length > 2048) {
        length = 2048;
    }
    if(type == DataType::ACCELEROMETER)
        packAccelometerData(data);
    else if(type == DataType::LIDAR)
        memcpy(packedData, data, length);
}

void Packet::packAccelometerData(const uint8_t* data){
    packedData[0] = 0x00;
    packedData[1] = 0x00;
    packedData[2] = length & 0xFF;
    packedData[3] = length >> 8;

    memcpy(packedData + 4, data, length);
}