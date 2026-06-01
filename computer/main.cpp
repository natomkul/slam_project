#include "wrapper.h"

#define PORT "3000"

int main(int argc, char** argv)
{
    auto w = std::make_unique<Wrapper>(argc, argv, PORT);

    if (!w->sendORrecv(1)) //parzysta -> ruch | nieparzysta -> odbieranie z czujnikow
    {
        return -1;
    }

    return 0;
}
