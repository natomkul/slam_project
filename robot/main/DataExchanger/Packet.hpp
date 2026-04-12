#pragma once

#include <cstdint>
#include <cstring>

enum class DataType{
    LIDAR,
    ACCELEROMETER
};

struct Packet {
    DataType type;
    uint16_t length;
    uint8_t packedData[2056];

    Packet() = default;
    Packet(DataType type, uint16_t length, const uint8_t* data) : type(type), length(length)
    {
        if (length > 2048) {
            this->length = 2048;
        }
        packedData[0] = 0x00;
        packedData[1] = 0x00;
        packedData[2] = length & 0xFF;
        packedData[3] = length >> 8;

        memcpy(packedData + 4, data, length);
    }
};