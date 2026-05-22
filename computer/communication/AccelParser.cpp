#include "AccelParser.h"

AccelData::AccelData(socket_t cfd, socket_t sfd) : cfd(cfd), sfd(sfd)
{
    output = AccelRecv();
}

void AccelData::printAccel()
{
    printf("===== ACCEL DATA =====\n");

    printf("Magnetometer:\n");
    printf("  mx: %d\n", data.mx);
    printf("  my: %d\n", data.my);
    printf("  mz: %d\n", data.mz);

    printf("  rhall: %d\n", data.rhall);

    printf("Gyroscope:\n");
    printf("  gx: %u\n", data.gx);
    printf("  gy: %u\n", data.gy);
    printf("  gz: %u\n", data.gz);

    printf("Accelerometer:\n");
    printf("  ax: %u\n", data.ax);
    printf("  ay: %u\n", data.ay);
    printf("  az: %u\n", data.az);

    printf("======================\n");
}

bool AccelData::AccelRecv()
{
    uint8_t buf[sizeof(AccData)];
    int ret;

#ifdef _WIN32
    ret = recv(cfd, (char*)buf, sizeof(buf), 0);

    if (ret < 0)
    {
        printf("Accel recv error: %d\n", WSAGetLastError());
        closesocket(cfd);
        closesocket(sfd);

        WSACleanup();
#else
    ret = recv(cfd, &buf, sizeof(buf), 0);
    
    if (ret  < 0)
    {
        perror("Accel recv");
        close(cfd);
        close(sfd);
#endif
        return false;

    } else if (ret == 0){

        return true;
    }
    
    int offset = 0;

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

    printAccel();

    return true;
}

bool AccelData::get_output()
{
    return output;
}

AccData AccelData::get_data()
{
    return data;
}
