#include <iostream>

#define POINT_PER_PACK 12

struct LidarPoints
{
    uint16_t distanceValue;
    uint8_t confidence;
};

struct LidarData
{
    int8_t      dataLength;
    uint16_t    speed;
    uint16_t    startAngle;
    LidarPoints point[POINT_PER_PACK];
    uint16_t    endAngle;
    uint16_t    timestamp;
    uint8_t     crc8;
};
