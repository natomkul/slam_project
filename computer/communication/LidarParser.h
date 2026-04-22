#include <iostream>
#include <cstdint>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define POINT_PER_PACK 12
#define Li_DATA_SIZE sizeof(LidarData)

struct LidarPoints
{
    uint16_t distanceValue;
    uint8_t confidence;
};

class LidarData
{
 private:
    int8_t      dataLength;
    uint16_t    speed;
    uint16_t    startAngle;
    LidarPoints point[POINT_PER_PACK];
    uint16_t    endAngle;
    uint16_t    timestamp;
    uint8_t     crc8;

    int cfd, sfd;
    bool output;
 public:
    LidarData() = delete;
    LidarData(int cfd, int sfd);

    void printLidar();
    bool LidarRecv();
    bool get_output();
};
