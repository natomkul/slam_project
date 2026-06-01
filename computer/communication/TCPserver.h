#pragma once

#include <variant>
#include <expected>

#include "LidarParser.h"
#include "AccelParser.h"
 
#define BACKLOG 10

#define LIDAR_TYPE 0x54
#define ACCEL_TYPE 0x67

class TCPserver
{
 private:
    socket_t cfd;
    socket_t sfd;
    const char* PORT;

 public:
    using Result = std::variant<std::monostate, LiData, AccData>;
    using FResult = std::expected<Result, std::error_code>;

    TCPserver(const char* PORT);
    ~TCPserver();

    AccData AccelHandl();
    LiData LidarHandl();
 
    bool connectSock();
    FResult receiveData();

    bool send_motion(const char* msg);
};

