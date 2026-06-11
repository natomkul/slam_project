#include "AccelParser.h"
#include <chrono>

namespace
{
constexpr int ACCEL_PAYLOAD_SIZE = 12 + 8;
constexpr uint64_t MAX_ACCEL_DT_NS = 200000000;

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

bool isSaneAccelFrame(const AccData& data)
{
    if (data.timestamp_delta_ns == 0 || data.timestamp_delta_ns > MAX_ACCEL_DT_NS)
    {
        return false;
    }

    if (data.gx == INT16_MIN || data.gy == INT16_MIN || data.gz == INT16_MIN ||
        data.ax == INT16_MIN || data.ay == INT16_MIN || data.az == INT16_MIN)
    {
        return false;
    }

    const int64_t accel_magnitude_sq =
        (int64_t)data.ax * data.ax +
        (int64_t)data.ay * data.ay +
        (int64_t)data.az * data.az;

    return accel_magnitude_sq >= 4000000 && accel_magnitude_sq <= 900000000;
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

    int offset = 0;

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

    if (!isSaneAccelFrame(data))
    {
        printf("Discarding invalid accel frame: gx=%d gy=%d gz=%d ax=%d ay=%d az=%d timestamp_delta_ns=%llu\n",
               data.gx,
               data.gy,
               data.gz,
               data.ax,
               data.ay,
               data.az,
               (unsigned long long)data.timestamp_delta_ns);
        return true;
    }

    writePacketDump(packet_type, buf, sizeof(buf));

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
