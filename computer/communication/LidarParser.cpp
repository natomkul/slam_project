#include "LidarParser.h"

LidarData::LidarData(socket_t cfd, socket_t sfd) : cfd(cfd), sfd(sfd)
{
    output = LidarRecv();
}

void LidarData::printLidar()
{
    printf("===== LIDAR PACKET =====\n");

    printf("dataLength: %u\n", data.dataLength);
    printf("speed: %u\n", data.speed);
    printf("startAngle: %u\n", data.startAngle);

    printf("points:\n");

    for (int i = 0; i < POINT_PER_PACK; i++)
    {
        printf("  [%d] dist=%u conf=%u\n",
               i,
               data.point[i].distanceValue,
               data.point[i].confidence);
    }

    printf("endAngle: %u\n", data.endAngle);
    printf("timestamp: %u\n", data.timestamp);
    printf("crc: %u\n", data.crc8);
    
    printf("========================\n");
}

bool LidarData::LidarRecv()
{
    uint8_t buf[sizeof(data)];
    int ret;

#ifdef _WIN32
    ret = recv(cfd, (char*)buf, sizeof(buf), 0);

    if (ret < 0)
    {
        printf("Lidar recv error: %d\n", WSAGetLastError());
        closesocket(cfd);
        closesocket(sfd);

        WSACleanup();
#else
    ret = recv(cfd, &buf, sizeof(data), 0);
    
    if (ret < 0)
    {
        perror("Lidar recv");
        close(cfd);
        close(sfd);
#endif
        return false;
    
    } else if (ret == 0) {
    
        return true;
    }
    
    int offset = 0;

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
    
    data.crc8 = buf[offset];
    
    printLidar();

    return true;
}

bool LidarData::get_output()
{
    return output;
}
