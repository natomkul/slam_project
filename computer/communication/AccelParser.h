#pragma once

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

#define Acc_DATA_SIZE sizeof(AccelData)

class AccelData
{
 private:
    int16_t mx, my, mz;
    int16_t rhall;
    int16_t gx, gy, gz;
    int16_t ax, ay, az;

    int cfd, sfd;
    bool output;
 public:
    AccelData() = delete;
    AccelData(int cfd, int sfd);
    
    void printAccel();
    bool AccelRecv();
    bool get_output();
};
