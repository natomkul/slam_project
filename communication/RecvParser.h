#pragma once

#include "LidarParser.h"
#include "AccelParser.h"

#include <vector>

#define LIDAR_TYPE 0x54
#define ACCEL_TYPE 0x67
#define Li_DATA_SIZE sizeof(LidarData)
#define Acc_DATA_SIZE sizeof(AccelData)

void printLidar(const LidarData& data)
{
    printf("===== LIDAR PACKET =====\n");

    printf("dataLength: %d\n", (int)data.dataLength);
    printf("speed: %u\n", data.speed);
    printf("startAngle: %u\n", data.startAngle);

    printf("points:\n");

    for (int i = 0; i < POINT_PER_PACK; i++)
    {
        printf("  [%d] dist=%u conf=%d\n",
               i,
               data.point[i].distanceValue,
               (int)data.point[i].confidence);
    }

    printf("endAngle: %u\n", data.endAngle);
    printf("timestamp: %u\n", data.timestamp);
    printf("crc: %u\n", data.crc8);
    
    printf("========================\n");
}

int LidarRecv(int cfd, int sfd, std::vector<LidarData> LidarLoaded)
{
    uint8_t buf[Li_DATA_SIZE];

    if ((recv(cfd, &buf, Li_DATA_SIZE, 0)) <= 0)
    {
        perror("Lidar recv");
        close(cfd);
        close(sfd);
        return -1;
    }
    
    int offset = 0;
    LidarData data;

    data.dataLength = buf[offset];
    offset += sizeof(uint8_t);

    data.speed = (uint16_t) buf[offset];
    offset += sizeof(uint16_t);

    data.startAngle = (uint16_t) buf[offset];
    offset += sizeof(uint16_t);

    for (int i = 0; i < POINT_PER_PACK; i++)
    {
        data.point[i].distanceValue = (uint16_t) buf[offset];
        offset += sizeof(uint16_t);

        data.point[i].confidence = buf[offset];
        offset += sizeof(uint8_t);
    }

    data.endAngle = (uint16_t) buf[offset];
    offset += sizeof(uint16_t);

    data.timestamp = (uint16_t) buf[offset];
    offset += sizeof(uint16_t);
    
    uint8_t crc8 = buf[offset];
    
    printLidar(data);
    LidarLoaded.push_back(data);

    return 0;
}

int AccelRecv(int cfd, int sfd)
{
    uint16_t buf[Acc_DATA_SIZE];

    if ((recv(cfd, &buf, Acc_DATA_SIZE, 0)) <= 0)
    {
        perror("Accel recv");
        close(cfd);
        close(sfd);
        return -1;
    }
    
    int offset = 0;
    AccelData data;

    data.mx = buf[offset];
    offset += sizeof(uint16_t);
    
    data.my = buf[offset];
    offset += sizeof(uint16_t);
    
    data.mz = buf[offset];
    offset += sizeof(uint16_t);
    
    data.rhall = buf[offset];
    offset += sizeof(uint16_t);
    
    data.gx = buf[offset];
    offset += sizeof(uint16_t);
    
    data.gy = buf[offset];
    offset += sizeof(uint16_t);
 
    data.gz = buf[offset];
    offset += sizeof(uint16_t);
   
    data.ax = buf[offset];
    offset += sizeof(uint16_t);
    
    data.ay = buf[offset];
    offset += sizeof(uint16_t);
    
    data.az = buf[offset];
    offset += sizeof(uint16_t);
 
    return 0;
}

int PayloadRecv(int cfd, int sfd)
{

    std::vector<LidarData> LidarLoaded;
    std::vector<AccelData> AccelLoaded;

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

        switch(type)
        {
            case LIDAR_TYPE: LidarRecv(cfd, sfd, LidarLoaded); break;
            case ACCEL_TYPE: AccelRecv(cfd, sfd); break;
            default: return -1;
        }
    }
}

