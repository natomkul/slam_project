#include "TCPserver.h"

#define ROBOT_PORT "3000"
#define NAV_CMD_PORT "5006"

int main()
{
    TCPserver server(ROBOT_PORT, NAV_CMD_PORT);

    if (!server.connectSock())
    {
        return -1;
    }

    if (!server.initNavCommandServer())
    {
        return -1;
    }

    while (server.poll())
    {
    }

    return 0;
}
