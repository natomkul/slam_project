#pragma once

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
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

struct EnData
{
    uint8_t LorR; // wynik 1 || 2 (Lewy || Prawy)
    float meters;
};

class EncoderData
{
 private:
    EnData data;

    socket_t cfd, sfd;
    bool output;
 public:
    EncoderData() = delete;
    EncoderData(socket_t cfd, socket_t sfd);
    
    void printEncoder();
    bool EncoderRecv();
    bool get_output();
    EnData get_data();
};

