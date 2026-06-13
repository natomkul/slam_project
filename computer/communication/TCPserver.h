#pragma once

#include "LidarParser.h"
#include "AccelParser.h"
#include "EncoderParser.h"
#include "Request.h"

#include <string>

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

    socket_t nav_sfd;
    socket_t nav_cfd;
    const char* navPort;
    std::string navLineBuffer;

    bool navClientConnected_;
    bool robotClientConnected_;
    bool warnedNavNoRobot_;

    size_t navCommandsReceived_;
    bool loggedNavFirstRx_;

    void closeNavClient(const char* reason);
    void closeRobotClient(const char* reason);

    bool setNonBlocking(socket_t fd);
    bool tryAcceptRobotClient();
    bool tryAcceptNavClient();
    bool handleNavInput();
    bool sendRequestToRobot(const RequestWire& request);
    bool handleRobotInput();

 public:
    TCPserver(const char* PORT, const char* navPort = "5006");
    ~TCPserver();

    bool AccelHandl();
    bool LidarHandl();
    bool EncoderHandl();

    bool connectSock();
    bool initNavCommandServer();
    bool poll();
};
