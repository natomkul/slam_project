#pragma once

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>

#ifdef _WIN32
    #define _WINSOCK_DEPRECATED_NO_WARNINGS

    #include <winsock2.h>
    #include <ws2tcpip.h>

    #pragma comment(lib, "ws2_32.lib")

    using socket_t = SOCKET;

#else
    #include <unistd.h>
    #include <errno.h>

    #include <netdb.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>

    using socket_t = int;

#endif

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

    socket_t cfd, sfd;
    bool output;
 public:
    LidarData() = delete;
    LidarData(socket_t cfd, socket_t sfd);

    void printLidar();
    bool LidarRecv();
    bool get_output();
};
