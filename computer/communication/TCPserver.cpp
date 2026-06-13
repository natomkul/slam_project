#include "TCPserver.h"
#include "NavCommandParser.h"

#include <cerrno>
#include <cstdio>
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <fcntl.h>
    #include <sys/select.h>
    #include <unistd.h>
#endif

namespace
{
#ifdef _WIN32
bool isWouldBlock()
{
    const int err = WSAGetLastError();
    return err == WSAEWOULDBLOCK || err == WSAEINTR;
}
#else
bool isWouldBlock()
{
    return errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR;
}
#endif
void logMatlabNavCommand(const std::string& line, const RequestWire& request, size_t index)
{
    if (request.type == RequestType::move)
    {
        printf("[nav] #%zu from MATLAB: MOVE %.3f m  (raw: %s)\n",
               index,
               request.distanceInM,
               line.c_str());
        return;
    }

    if (request.type == RequestType::rotate)
    {
        printf("[nav] #%zu from MATLAB: ROTATE %.3f rad (%.1f deg)  (raw: %s)\n",
               index,
               request.rotationAngle,
               request.rotationAngle * 57.2957795f,
               line.c_str());
        return;
    }

    printf("[nav] #%zu from MATLAB: unknown  (raw: %s)\n", index, line.c_str());
}

void logMatlabNavIgnoredLine(const std::string& line)
{
    if (line.empty())
    {
        return;
    }
    printf("[MATLAB nav] ignored (bad format): \"%s\"\n", line.c_str());
}
}

TCPserver::TCPserver(const char* PORT, const char* navPort)
    : PORT(PORT),
      sfd(-1),
      cfd(-1),
      nav_sfd(-1),
      nav_cfd(-1),
      navPort(navPort),
      navClientConnected_(false),
      robotClientConnected_(false),
      warnedNavNoRobot_(false),
      navCommandsReceived_(0),
      loggedNavFirstRx_(false)
{
    resetPacketDump();

#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup failed\n");
    }
#endif
}

TCPserver::~TCPserver()
{
#ifdef _WIN32
    if (nav_cfd != -1)
    {
        closesocket(nav_cfd);
    }
    if (nav_sfd != -1)
    {
        closesocket(nav_sfd);
    }
    if (cfd != -1)
    {
        closesocket(cfd);
    }
    if (sfd != -1)
    {
        closesocket(sfd);
    }
    WSACleanup();
#else
    if (nav_cfd != -1)
    {
        close(nav_cfd);
    }
    if (nav_sfd != -1)
    {
        close(nav_sfd);
    }
    if (cfd != -1)
    {
        close(cfd);
    }
    if (sfd != -1)
    {
        close(sfd);
    }
#endif
}

void TCPserver::closeNavClient(const char* reason)
{
    if (nav_cfd == -1)
    {
        return;
    }

#ifdef _WIN32
    closesocket(nav_cfd);
#else
    close(nav_cfd);
#endif
    nav_cfd = -1;
    navLineBuffer.clear();

    if (navClientConnected_ && reason != nullptr)
    {
        printf("%s\n", reason);
    }
    navClientConnected_ = false;
    loggedNavFirstRx_ = false;
}

void TCPserver::closeRobotClient(const char* reason)
{
    if (cfd == -1)
    {
        return;
    }

#ifdef _WIN32
    closesocket(cfd);
#else
    close(cfd);
#endif
    cfd = -1;

    if (robotClientConnected_ && reason != nullptr)
    {
        printf("%s\n", reason);
    }
    robotClientConnected_ = false;
}

bool TCPserver::setNonBlocking(socket_t fd)
{
#ifdef _WIN32
    u_long mode = 1;
    return ioctlsocket(fd, FIONBIO, &mode) == 0;
#else
    const int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        return false;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK) != -1;
#endif
}

bool TCPserver::connectSock()
{
    int status;
    struct addrinfo hints{}, *servinfo, *p;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        perror("getaddrinfo");
        return false;
    }

    const int yes = 1;

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("socket");
            continue;
        }

        printf("Robot sensor server on port %s\n", PORT);

        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            return false;
        }

        if (bind(sfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            perror("server: bind");
#ifdef _WIN32
            closesocket(sfd);
#else
            close(sfd);
#endif
            sfd = -1;
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (p == NULL || sfd == -1)
    {
        perror("server: failed to bind");
        return false;
    }

    if (listen(sfd, BACKLOG) == -1)
    {
        perror("listen");
        return false;
    }

    if (!setNonBlocking(sfd))
    {
        perror("robot listen nonblocking");
        return false;
    }

    printf("Waiting for robot on port %s...\n", PORT);
    return true;
}

bool TCPserver::tryAcceptRobotClient()
{
    if (sfd == -1 || cfd != -1)
    {
        return true;
    }

    struct sockaddr_in cAddr{};

#ifdef _WIN32
    int sin_size = sizeof(cAddr);
#else
    socklen_t sin_size = sizeof cAddr;
#endif

    const socket_t client = accept(sfd, (struct sockaddr*)&cAddr, &sin_size);
    if (client == -1)
    {
        if (isWouldBlock())
        {
            return true;
        }
        perror("accept robot");
        return true;
    }

    cfd = client;
    setNonBlocking(cfd);
    robotClientConnected_ = true;
    warnedNavNoRobot_ = false;
    printf("Robot connected\n");
    return true;
}

bool TCPserver::initNavCommandServer()
{
    if (navPort == nullptr)
    {
        return false;
    }

    struct addrinfo hints{}, *servinfo, *p;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    const int status = getaddrinfo(NULL, navPort, &hints, &servinfo);
    if (status != 0)
    {
        perror("getaddrinfo nav");
        return false;
    }

    const int yes = 1;

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        nav_sfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (nav_sfd == -1)
        {
            perror("socket nav");
            continue;
        }

        if (setsockopt(nav_sfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(int)) == -1)
        {
            perror("setsockopt nav");
            return false;
        }

        if (bind(nav_sfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            perror("bind nav");
#ifdef _WIN32
            closesocket(nav_sfd);
#else
            close(nav_sfd);
#endif
            nav_sfd = -1;
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (p == NULL || nav_sfd == -1)
    {
        fprintf(stderr, "Nav command server: failed to bind port %s\n", navPort);
        return false;
    }

    if (listen(nav_sfd, BACKLOG) == -1)
    {
        perror("listen nav");
        return false;
    }

    if (!setNonBlocking(nav_sfd))
    {
        perror("nav nonblocking");
        return false;
    }

    printf("MATLAB nav commands listening on port %s\n", navPort);
    return true;
}

bool TCPserver::tryAcceptNavClient()
{
    if (nav_sfd == -1 || nav_cfd != -1)
    {
        return true;
    }

    struct sockaddr_in addr{};
#ifdef _WIN32
    int len = sizeof(addr);
#else
    socklen_t len = sizeof(addr);
#endif

    const socket_t client = accept(nav_sfd, (struct sockaddr*)&addr, &len);
    if (client == -1)
    {
#ifdef _WIN32
        if (isWouldBlock())
#else
        if (isWouldBlock())
#endif
        {
            return true;
        }
        perror("accept nav");
        return true;
    }

    nav_cfd = client;
    setNonBlocking(nav_cfd);
    navClientConnected_ = true;
    navCommandsReceived_ = 0;
    loggedNavFirstRx_ = false;
    printf("[nav] MATLAB connected on port %s — ready for move/rotate lines\n", navPort);
    fflush(stdout);
    return true;
}

bool TCPserver::sendRequestToRobot(const RequestWire& request)
{
    if (cfd == -1)
    {
        if (!warnedNavNoRobot_)
        {
            fprintf(stderr,
                    "[MATLAB nav] robot not connected — commands are logged only until robot connects\n");
            warnedNavNoRobot_ = true;
        }
        return false;
    }

    const char* bytes = reinterpret_cast<const char*>(&request);
    const size_t total = sizeof(request);
    size_t sent = 0;

    while (sent < total)
    {
#ifdef _WIN32
        const int n = send(cfd, bytes + sent, (int)(total - sent), 0);
#else
        const ssize_t n = send(cfd, bytes + sent, total - sent, 0);
#endif
        if (n <= 0)
        {
            if (isWouldBlock())
            {
                continue;
            }
            closeRobotClient("Robot disconnected (send failed)");
            return false;
        }
        sent += (size_t)n;
    }

    if (request.type == RequestType::move)
    {
        printf("  -> sent to robot: MOVE %.3f m\n", request.distanceInM);
    }
    else if (request.type == RequestType::rotate)
    {
        printf("  -> sent to robot: ROTATE %.3f rad\n", request.rotationAngle);
    }

    return true;
}

bool TCPserver::handleNavInput()
{
    char chunk[256];

#ifdef _WIN32
    const int n = recv(nav_cfd, chunk, sizeof(chunk) - 1, 0);
#else
    const ssize_t n = recv(nav_cfd, chunk, sizeof(chunk) - 1, 0);
#endif

    if (n == 0)
    {
        closeNavClient("MATLAB nav client disconnected");
        return true;
    }

    if (n < 0)
    {
        if (isWouldBlock())
        {
            return true;
        }
        closeNavClient("MATLAB nav client disconnected (recv error)");
        return true;
    }

    chunk[n] = '\0';
    navLineBuffer.append(chunk, (size_t)n);

    if (!loggedNavFirstRx_)
    {
        printf("[nav] receiving data from MATLAB (%d bytes)\n", (int)n);
        loggedNavFirstRx_ = true;
        fflush(stdout);
    }

    for (;;)
    {
        const size_t nl = navLineBuffer.find('\n');
        if (nl == std::string::npos)
        {
            break;
        }

        std::string line = navLineBuffer.substr(0, nl);
        navLineBuffer.erase(0, nl + 1);

        RequestWire req{};
        if (parseNavCommandLine(line, req))
        {
            navCommandsReceived_++;
            logMatlabNavCommand(line, req, navCommandsReceived_);
            fflush(stdout);
            sendRequestToRobot(req);
        }
        else
        {
            logMatlabNavIgnoredLine(line);
            fflush(stdout);
        }
    }

    return true;
}

bool TCPserver::handleRobotInput()
{
    uint8_t type{};
#ifdef _WIN32
    const int ret = recv(cfd, (char*)&type, sizeof(type), 0);
#else
    const ssize_t ret = recv(cfd, &type, sizeof(uint8_t), 0);
#endif

    if (ret == 0)
    {
        closeRobotClient("Robot disconnected");
        return true;
    }

    if (ret < 0)
    {
        if (isWouldBlock())
        {
            return true;
        }
        closeRobotClient("Robot disconnected (recv error)");
        return true;
    }

    bool output = true;

    switch (type)
    {
        case LIDAR_TYPE: output = LidarHandl(); break;
        case ACCEL_TYPE: output = AccelHandl(); break;
        case ENCODER_TYPE: output = EncoderHandl(); break;
        default: break;
    }

    return output;
}

bool TCPserver::poll()
{
    fd_set readfds;
    FD_ZERO(&readfds);

    socket_t maxfd = 0;

    if (sfd != -1 && cfd == -1)
    {
        FD_SET(sfd, &readfds);
        maxfd = sfd;
    }

    if (cfd != -1)
    {
        FD_SET(cfd, &readfds);
        maxfd = cfd;
    }

    if (nav_cfd != -1)
    {
        FD_SET(nav_cfd, &readfds);
        if (nav_cfd > maxfd)
        {
            maxfd = nav_cfd;
        }
    }

    if (nav_sfd != -1 && nav_cfd == -1)
    {
        FD_SET(nav_sfd, &readfds);
        if (nav_sfd > maxfd)
        {
            maxfd = nav_sfd;
        }
    }

    timeval timeout{};
    timeout.tv_sec = 0;
    timeout.tv_usec = 50000;

#ifdef _WIN32
    if (maxfd == 0)
    {
        Sleep(50);
        return true;
    }
#endif

    const int ready = select((int)maxfd + 1, &readfds, nullptr, nullptr, &timeout);
    if (ready < 0)
    {
        perror("select");
        return false;
    }

    if (ready == 0)
    {
        return true;
    }

    if (sfd != -1 && cfd == -1 && FD_ISSET(sfd, &readfds))
    {
        tryAcceptRobotClient();
    }

    if (nav_sfd != -1 && nav_cfd == -1 && FD_ISSET(nav_sfd, &readfds))
    {
        tryAcceptNavClient();
    }

    if (nav_cfd != -1 && FD_ISSET(nav_cfd, &readfds))
    {
        if (!handleNavInput())
        {
            return false;
        }
    }

    if (cfd != -1 && FD_ISSET(cfd, &readfds))
    {
        if (!handleRobotInput())
        {
            return false;
        }
    }

    return true;
}

bool TCPserver::AccelHandl()
{
    AccelData data(cfd, sfd, ACCEL_TYPE);

    return data.get_output();
}

bool TCPserver::LidarHandl()
{
    LidarData data(cfd, sfd, LIDAR_TYPE);

    return data.get_output();
}

bool TCPserver::EncoderHandl()
{
    EncoderData data(cfd, sfd, ENCODER_TYPE);

    return data.get_output();
}
