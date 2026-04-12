#pragma once

#include <cstdint>
#include <cstring>

enum class DataType{
    LIDAR,
    ACCELEROMETER
};

class Packet {
public:
    DataType type;
    uint16_t length;
    uint8_t packedData[2048];

    Packet() = default;
    Packet(DataType type, uint16_t length, const uint8_t* data);
    ~Packet() = default;
    void packAccelometerData(const uint8_t* data);
};