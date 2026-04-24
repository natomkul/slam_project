#include "AccelParser.h"

AccelData::AccelData(socket_t cfd, socket_t sfd) : cfd(cfd), sfd(sfd)
{
    output = AccelRecv();
}

void AccelData::printAccel()
{
    printf("===== ACCEL DATA =====\n");

    printf("Magnetometer:\n");
    printf("  mx: %d\n", mx);
    printf("  my: %d\n", my);
    printf("  mz: %d\n", mz);

    printf("  rhall: %d\n", rhall);

    printf("Gyroscope:\n");
    printf("  gx: %u\n", gx);
    printf("  gy: %u\n", gy);
    printf("  gz: %u\n", gz);

    printf("Accelerometer:\n");
    printf("  ax: %u\n", ax);
    printf("  ay: %u\n", ay);
    printf("  az: %u\n", az);

    printf("======================\n");
}

bool AccelData::AccelRecv()
{
    uint8_t buf[Acc_DATA_SIZE];


#ifdef _WIN32
    int ret = recv(cfd, (char*)buf, sizeof(buf), 0);

    if (ret <= 0)
    {
        printf("Accel recv error: %d\n", WSAGetLastError());
        closesocket(cfd);
        closesocket(sfd);

        WSACleanup();
#else
    if ((recv(cfd, &buf, sizeof(buf), 0)) <= 0)
    {
        perror("Accel recv");
        close(cfd);
        close(sfd);
#endif
        return false;
    }
    
    int offset = 0;

    mx = buf[offset];
    offset += sizeof(uint16_t);
    
    my = buf[offset];
    offset += sizeof(uint16_t);
    
    mz = buf[offset];
    offset += sizeof(uint16_t);
    
    rhall = buf[offset];
    offset += sizeof(uint16_t);
    
    gx = buf[offset];
    offset += sizeof(uint16_t);
    
    gy = buf[offset];
    offset += sizeof(uint16_t);
 
    gz = buf[offset];
    offset += sizeof(uint16_t);
   
    ax = buf[offset];
    offset += sizeof(uint16_t);
    
    ay = buf[offset];
    offset += sizeof(uint16_t);
    
    az = buf[offset];
    offset += sizeof(uint16_t);

    printAccel();

    return true;
}

bool AccelData::get_output()
{
    return output;
}
