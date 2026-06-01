#include "move.h"
#include "SensorsNode.h"

class Wrapper
{
 private:
    std::shared_ptr<TCPserver> server;
    std::shared_ptr<SensorsNode> node;
    std::shared_ptr<Move> move;
 
    bool conn();
    bool work_node();
    bool send_move();
 public:
    Wrapper(int argc, char** argv, const char* PORT);

    bool sendORrecv(int num);
};
