#include "LidarParser.h"

LidarData::LidarData(int cfd, int sfd) : cfd(cfd), sfd(sfd)
{
    output = LidarRecv();
}

void LidarData::printLidar()
{
    printf("===== LIDAR PACKET =====\n");

    printf("dataLength: %d\n", (int)dataLength);
    printf("speed: %u\n", speed);
    printf("startAngle: %u\n", startAngle);

    printf("points:\n");

    for (int i = 0; i < POINT_PER_PACK; i++)
    {
        printf("  [%d] dist=%u conf=%d\n",
               i,
               point[i].distanceValue,
               (int)point[i].confidence);
    }

    printf("endAngle: %u\n", endAngle);
    printf("timestamp: %u\n", timestamp);
    printf("crc: %u\n", crc8);
    
    printf("========================\n");
}

bool LidarData::LidarRecv()
{
    uint8_t buf[Li_DATA_SIZE];

    if ((recv(cfd, &buf, Li_DATA_SIZE, 0)) <= 0)
    {
        perror("Lidar recv");
#ifdef _WIN32
        closesocket(cfd);
        closesocket(sfd);

        WSACleanup();
#else
        close(cfd);
        close(sfd);
#endif
        return false;
    }
    
    int offset = 0;

    dataLength = buf[offset];
    offset += sizeof(uint8_t);

    speed = (uint16_t) buf[offset];
    offset += sizeof(uint16_t);

    startAngle = (uint16_t) buf[offset];
    offset += sizeof(uint16_t);

    for (int i = 0; i < POINT_PER_PACK; i++)
    {
        point[i].distanceValue = (uint16_t) buf[offset];
        offset += sizeof(uint16_t);

        point[i].confidence = buf[offset];
        offset += sizeof(uint8_t);
    }

    endAngle = (uint16_t) buf[offset];
    offset += sizeof(uint16_t);

    timestamp = (uint16_t) buf[offset];
    offset += sizeof(uint16_t);
    
    uint8_t crc8 = buf[offset];
    
    printLidar();

    return true;
}

bool LidarData::get_output()
{
    return output;
}
