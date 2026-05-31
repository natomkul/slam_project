#include "SensorsNode.h"

#define PORT "3000"

int main(int argc, char** argv)
{
    auto server = std::make_unique<TCPserver>(PORT);
     
    rclcpp::init(argc, argv);

    auto node = std::make_shared<SensorsNode>(server.get());

    node->start();

    rclcpp::spin(node);

    node->stop();

    rclcpp::shutdown();
    return 0;
}
