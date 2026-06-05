#pragma once

#include <variant>
#include <expected>

#include "LidarParser.h"
#include "AccelParser.h"
#include "EncoderParser.h"
 
#define BACKLOG 10

#define LIDAR_TYPE 0x54
#define ACCEL_TYPE 0x67
#define ENCODER_TYPE 0x68

class TCPserver
{
 private:
    socket_t cfd;
    socket_t sfd;
    const char* PORT;

 public:
    using Result = std::variant<std::monostate, LiData, AccData, EnData>;
    using FResult = std::expected<Result, std::error_code>;

    TCPserver(const char* PORT);
    ~TCPserver();

    AccData AccelHandl();
    LiData LidarHandl();
    EnData EncoderHandl();
 
    bool connectSock();
    FResult receiveData();

    bool send_motion(const char* msg);
};

