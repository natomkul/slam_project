#include <iostream>
#include <memory>
#include "TCPserver.h"

#ifdef _WIN32
    #include <conio.h>
#else
    #include <unistd.h>
    #include <termios.h>
#endif

enum class Direction {
    FORWARD,
    BACKWARD,
    CW,
    COUNTERCW,
    UNKNOWN
};

class Move
{
 private:
    std::shared_ptr<TCPserver> server;
#ifdef _WIN32
    char get_inputWind();
#else
    char get_input();
#endif
 public:
    Move(std::shared_ptr<TCPserver> server);
    const char* print_input();
};

