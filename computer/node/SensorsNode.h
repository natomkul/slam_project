#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "TCPserver.h"
#include <thread>
#include <atomic>

class SensorsNode : public rclcpp::Node
{
 private:
    rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr scan_pub_;
    sensor_msgs::msg::LaserScan LidarDataToScan(const LiData &data);

    TCPserver* server;
    
    std::thread proc_thread_;
    std::atomic<bool> running_{false};

    bool proc();
 public:
    SensorsNode(const char* PORT);

    void publish_scan(const LiData& data);

    void start();
    void stop();
};

