#include <winsock2.h>
#include <ws2tcpip.h>

#include <array>
#include <cmath>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

namespace
{
constexpr uint8_t LIDAR_TYPE = 0x54;
constexpr uint8_t ACCEL_TYPE = 0x67;
constexpr uint8_t ENCODER_TYPE = 0x68;

constexpr size_t LIDAR_PACKET_SIZE = 47;
constexpr size_t ACCEL_PACKET_SIZE = 29;
constexpr size_t ENCODER_PACKET_SIZE = 14;
constexpr int POINT_PER_PACK = 12;
constexpr int MATLAB_RANGES = 360;

uint16_t readU16(const std::vector<uint8_t>& data, size_t offset)
{
    return (uint16_t)data[offset] | ((uint16_t)data[offset + 1] << 8);
}

int16_t readI16(const std::vector<uint8_t>& data, size_t offset)
{
    return (int16_t)readU16(data, offset);
}

float readFloat(const std::vector<uint8_t>& data, size_t offset)
{
    uint32_t bits =
        ((uint32_t)data[offset]) |
        ((uint32_t)data[offset + 1] << 8) |
        ((uint32_t)data[offset + 2] << 16) |
        ((uint32_t)data[offset + 3] << 24);

    float value;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

bool isPacketStart(uint8_t value)
{
    return value == LIDAR_TYPE || value == ACCEL_TYPE || value == ENCODER_TYPE;
}

size_t packetSize(uint8_t type)
{
    switch (type)
    {
        case LIDAR_TYPE: return LIDAR_PACKET_SIZE;
        case ACCEL_TYPE: return ACCEL_PACKET_SIZE;
        case ENCODER_TYPE: return ENCODER_PACKET_SIZE;
        default: return 0;
    }
}

std::vector<uint8_t> readDumpBytes(const std::string& path)
{
    std::ifstream in(path);
    std::vector<uint8_t> bytes;
    std::string token;

    while (in >> token)
    {
        unsigned int value;
        std::stringstream ss;
        ss << std::hex << token;

        if (ss >> value)
        {
            bytes.push_back((uint8_t)value);
        }
    }

    return bytes;
}

bool looksLikeLidar(const std::vector<uint8_t>& bytes, size_t offset)
{
    if (offset + LIDAR_PACKET_SIZE > bytes.size() || bytes[offset] != LIDAR_TYPE)
    {
        return false;
    }

    return bytes[offset + 1] == 0x2C;
}

bool looksLikeAccel(const std::vector<uint8_t>& bytes, size_t offset)
{
    return offset + ACCEL_PACKET_SIZE <= bytes.size() && bytes[offset] == ACCEL_TYPE;
}

bool looksLikeEncoder(const std::vector<uint8_t>& bytes, size_t offset)
{
    if (offset + ENCODER_PACKET_SIZE > bytes.size() || bytes[offset] != ENCODER_TYPE)
    {
        return false;
    }

    uint8_t encoder = bytes[offset + 1];
    return encoder >= 1 && encoder <= 3;
}

bool alignedAt(const std::vector<uint8_t>& bytes, size_t offset)
{
    return looksLikeLidar(bytes, offset) || looksLikeAccel(bytes, offset) || looksLikeEncoder(bytes, offset);
}

void decodeLidar(const std::vector<uint8_t>& packet, std::array<double, MATLAB_RANGES>& ranges)
{
    uint16_t startAngleRaw = readU16(packet, 4);
    uint16_t endAngleRaw = readU16(packet, 42);

    double start = (double)startAngleRaw / 100.0;
    double end = (double)endAngleRaw / 100.0;

    if (end < start)
    {
        end += 360.0;
    }

    for (int i = 0; i < POINT_PER_PACK; i++)
    {
        size_t pointOffset = 6 + i * 3;
        double distance = (double)readU16(packet, pointOffset) / 1000.0;
        double angle = start + (end - start) * (double)i / (double)(POINT_PER_PACK - 1);

        while (angle >= 360.0)
        {
            angle -= 360.0;
        }

        int index = (int)std::round(angle);
        if (index >= MATLAB_RANGES)
        {
            index = 0;
        }

        if (distance > 0.0)
        {
            ranges[index] = distance;
        }
    }
}

void decodeAccel(const std::vector<uint8_t>& packet, double& gyro)
{
    gyro = (double)readI16(packet, 13);
}

void decodeEncoder(const std::vector<uint8_t>& packet, double& distanceR, double& distanceL)
{
    uint8_t encoder = packet[1];
    double meters = (double)readFloat(packet, 2);

    if (encoder == 2)
    {
        meters = -meters;
    }

    if (encoder == 1)
    {
        distanceR = meters;
    }
    else if (encoder == 2)
    {
        distanceL = meters;
    }
}

std::string buildMatlabPacket(double distanceR, double distanceL, double gyro,
                              const std::array<double, MATLAB_RANGES>& ranges)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6);
    oss << distanceR << "," << distanceL << "," << gyro;

    for (double range : ranges)
    {
        oss << "," << range;
    }

    oss << "\n";
    return oss.str();
}
}

int main(int argc, char** argv)
{
    std::string dumpPath = argc > 1 ? argv[1] : "dump.txt";
    std::string host = argc > 2 ? argv[2] : "127.0.0.1";
    int port = argc > 3 ? std::atoi(argv[3]) : 5005;
    int delayMs = argc > 4 ? std::atoi(argv[4]) : 50;

    std::vector<uint8_t> dumpBytes = readDumpBytes(dumpPath);
    if (dumpBytes.empty())
    {
        std::cout << "Dump file is empty or not found: " << dumpPath << "\n";
        return -1;
    }

    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons((u_short)port);
    inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr);

    std::cout << "Connecting to MATLAB " << host << ":" << port << "...\n";

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cout << "Connection failed\n";
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    std::cout << "Connected!\n";

    std::array<double, MATLAB_RANGES> ranges{};
    ranges.fill(8.0);

    double distanceR = 0.0;
    double distanceL = 0.0;
    double gyro = 0.0;
    int sentPackets = 0;

    for (size_t i = 0; i < dumpBytes.size();)
    {
        if (!isPacketStart(dumpBytes[i]) || !alignedAt(dumpBytes, i))
        {
            i++;
            continue;
        }

        size_t size = packetSize(dumpBytes[i]);
        if (size == 0 || i + size > dumpBytes.size())
        {
            break;
        }

        std::vector<uint8_t> packet(dumpBytes.begin() + i, dumpBytes.begin() + i + size);

        if (packet[0] == LIDAR_TYPE)
        {
            decodeLidar(packet, ranges);

            std::string matlabPacket = buildMatlabPacket(distanceR, distanceL, gyro, ranges);
            send(sock, matlabPacket.c_str(), (int)matlabPacket.size(), 0);
            sentPackets++;
            Sleep((DWORD)delayMs);
        }
        else if (packet[0] == ACCEL_TYPE)
        {
            decodeAccel(packet, gyro);
        }
        else if (packet[0] == ENCODER_TYPE)
        {
            decodeEncoder(packet, distanceR, distanceL);
        }

        i += size;
    }

    send(sock, "STOP\n", 5, 0);
    std::cout << "Sent " << sentPackets << " MATLAB packets\n";

    closesocket(sock);
    WSACleanup();

    return 0;
}
