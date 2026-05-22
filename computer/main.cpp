#include "SensorsNode.h"

#define PORT "3000"

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<SensorsNode>(PORT);

    node->start();   // 🔥 TCP thread start

    rclcpp::spin(node);

    node->stop();    // cleanup

    rclcpp::shutdown();
    return 0;
}
