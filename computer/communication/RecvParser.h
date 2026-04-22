#pragma once

#include "LidarParser.h"
#include "AccelParser.h"

#include <vector>

#define LIDAR_TYPE 0x54
#define ACCEL_TYPE 0x67

bool AccelHandl(int cfd, int sfd)
{
    AccelData data(cfd, sfd);

    return data.get_output();
}

bool LidarHandl(int cfd, int sfd)
{
    LidarData data(cfd, sfd);

    return data.get_output();
}

int PayloadRecv(int cfd, int sfd)
{

    while (true)
    {
        uint8_t type;

        if ((recv(cfd, &type, sizeof(uint8_t), 0)) <= 0)
        {
            perror("type recv");
            close(cfd);
            close(sfd);
            return -1;
        }

        printf("recv data type: %d\n", type);

        bool output;

        switch(type)
        {
            case LIDAR_TYPE: output = LidarHandl(cfd, sfd); break;
            case ACCEL_TYPE: output = AccelHandl(cfd, sfd); break;
            default: return -1;
        }

        if (!output)
        {
            return -1;
        }
    }
}

