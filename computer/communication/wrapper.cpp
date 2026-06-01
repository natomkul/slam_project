#include "wrapper.h"

Wrapper::Wrapper(int argc, char** argv, const char* PORT)
{
    server = std::make_shared<TCPserver>(PORT);

    rclcpp::init(argc, argv);
    node = std::make_shared<SensorsNode>(server);

    move = std::make_shared<Move>(server);
}

bool Wrapper::conn()
{
    return server->connectSock();
}

bool Wrapper::work_node()
{
    node->start();

    rclcpp::spin(node);

    node->stop();

    rclcpp::shutdown();

    return true;
}

bool Wrapper::send_move()
{
    return server->send_motion(move->print_input());
}

bool Wrapper::sendORrecv(int num)
{
    bool out = conn();

    if (!out)
    {
        return out;
    }

    if (num % 2 == 0)
    {
        while (true)
        {
            out = send_move();

            if (!out)
            {
                return out;
            }
        }
    }
    else
    {
        while (true)
        {
            out = work_node();

            if (!out)
            {
                return out;
            }
        }
    }

    return true;
}

