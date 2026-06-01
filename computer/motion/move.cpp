#include "move.h"

Move::Move(std::shared_ptr<TCPserver> server) : server(server)
{
}

#ifdef _WIN32
char Move::get_inputWind()
{
    return _getch();
}

#else

char Move::get_input()
{
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    char ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

const char* Move::print_input()
{
    char in;
    Direction dir;
    const char* mesg;

#ifdef _WIN32
    in = get_inputWind();
#else
    in = get_input();
#endif
    switch (in) 
    {
        case 'w': case 'W':
            dir = Direction::FORWARD;
            break;
        case 's': case 'S':
            dir = Direction::BACKWARD;
            break;
        case 'a': case 'A':
            dir = Direction::COUNTERCW;
            break;
        case 'd': case 'D':
            dir = Direction::CW;
            break;
        default:
            dir = Direction::UNKNOWN;
            break;
    }

    switch (dir) {
        case Direction::FORWARD:
            mesg = "^\n|\n";
            break;
        case Direction::BACKWARD:
            mesg = "|\nv\n";
            break;
        case Direction::COUNTERCW:
            mesg = "<-\n";
            break;
        case Direction::CW:
            mesg = "->\n";
            break;
        case Direction::UNKNOWN:
            mesg = "UKNOWN\n";
            break;
    }

    return mesg;
}
