#include "AccelParser.h"
#include <chrono>

namespace
{
constexpr int ACCEL_PAYLOAD_SIZE = 20 + 8;

int16_t readI16(const uint8_t* buf, int offset)
{
    return (int16_t)((uint16_t)buf[offset] | ((uint16_t)buf[offset + 1] << 8));
}

uint64_t readU64(const uint8_t* buf, int offset)
{
    uint64_t value = 0;

    for (int i = 0; i < 8; i++)
    {
        value |= ((uint64_t)buf[offset + i]) << (8 * i);
    }

    return value;
}
}

AccelData::AccelData(socket_t cfd, socket_t sfd, uint8_t packet_type)
    : cfd(cfd), sfd(sfd), packet_type(packet_type)
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
    printf("  gx: %d\n", data.gx);
    printf("  gy: %d\n", data.gy);
    printf("  gz: %d\n", data.gz);

    printf("Accelerometer:\n");
    printf("  ax: %d\n", data.ax);
    printf("  ay: %d\n", data.ay);
    printf("  az: %d\n", data.az);

    printf("timestamp_delta_ns: %llu\n", (unsigned long long)data.timestamp_delta_ns);

    printf("======================\n");
}

bool AccelData::AccelRecv()
{
    uint8_t buf[ACCEL_PAYLOAD_SIZE];
    int received = 0;

    while (received < (int)sizeof(buf))
    {
        int ret;
#ifdef _WIN32
        ret = recv(cfd, (char*)buf + received, sizeof(buf) - received, 0);

        if (ret < 0)
        {
            printf("Accel recv error: %d\n", WSAGetLastError());
            closesocket(cfd);
            closesocket(sfd);

            WSACleanup();
#else
        ret = recv(cfd, buf + received, sizeof(buf) - received, 0);
    
        if (ret  < 0)
        {
            perror("Accel recv");
            close(cfd);
            close(sfd);
#endif
            return false;

        } else if (ret == 0){

            return false;
        }

        received += ret;
    }

    writePacketDump(packet_type, buf, sizeof(buf));
    
    int offset = 0;

    data.mx = readI16(buf, offset);
    offset += sizeof(uint16_t);
    
    data.my = readI16(buf, offset);
    offset += sizeof(uint16_t);
    
    data.mz = readI16(buf, offset);
    offset += sizeof(uint16_t);
    
    data.rhall = readI16(buf, offset);
    offset += sizeof(uint16_t);

    data.gx = readI16(buf, offset);
    offset += sizeof(uint16_t);
    
    data.gy = readI16(buf, offset);
    offset += sizeof(uint16_t);
 
    data.gz = readI16(buf, offset);
    offset += sizeof(uint16_t);
   
    data.ax = readI16(buf, offset);
    offset += sizeof(uint16_t);
    
    data.ay = readI16(buf, offset);
    offset += sizeof(uint16_t);
    
    data.az = readI16(buf, offset);
    offset += sizeof(uint16_t);

    data.timestamp_delta_ns = readU64(buf, offset);
    offset += sizeof(uint64_t);

    static auto last_print = std::chrono::steady_clock::now() - std::chrono::seconds(1);
    auto now = std::chrono::steady_clock::now();

    if (now - last_print >= std::chrono::seconds(1))
    {
        printAccel();
        last_print = now;
    }

    return true;
}

bool AccelData::get_output()
{
    return output;
}
