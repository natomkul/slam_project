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

#define Acc_DATA_SIZE sizeof(AccelData)

class AccelData
{
 private:
    int16_t mx, my, mz;
    int16_t rhall;
    int16_t gx, gy, gz;
    int16_t ax, ay, az;

    socket_t cfd, sfd;
    bool output;
 public:
    AccelData() = delete;
    AccelData(socket_t cfd, socket_t sfd);
    
    void printAccel();
    bool AccelRecv();
    bool get_output();
};
