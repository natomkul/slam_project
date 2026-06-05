#include "EncoderParser.h"

EncoderData::EncoderData(socket_t cfd, socket_t sfd) : cfd(cfd), sfd(sfd)
{
    output = EncoderRecv();
}

void EncoderData::printEncoder()
{
    printf("===== ENCODER DATA =====\n");

    if (data.LorR == 1)
    {
        printf("Left:\n");
    }
    else if (data.LorR == 2)
    {
        printf("Right:\n");
    }

    printf("  meters: %f\n", data.meters);
}

bool EncoderData::EncoderRecv()
{
    uint8_t buf[sizeof(EnData)];
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

    data.LorR = buf[0];
    offset += sizeof(uint8_t);

    uint32_t rawMeters = buf[1] |
                        (buf[2] << 8) |
                        (buf[3] << 16) |
                        (buf[4] << 24);

    data.meters = std::bit_cast<float>(rawMeters);

    printEncoder();

    return true;
}

bool EncoderData::get_output()
{
    return output;
}

EnData EncoderData::get_data()
{
    return data;
}

