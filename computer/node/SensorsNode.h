#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include "TCPserver.h"
#include <thread>
#include <atomic>

class SensorsNode : public rclcpp::Node
{
 private:
    rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr scan_pub_;
    sensor_msgs::msg::LaserScan LidarDataToScan(const LiData &data);

    rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr imu_pub_;
    sensor_msgs::msg::Imu AccelDataToImu(const AccData &data);

    TCPserver* server;
    
    std::thread proc_thread_;
    std::atomic<bool> running_{false};

    bool proc();
 public:
    SensorsNode(TCPserver* server);

    void publish_scan(const LiData& data);
    void publish_imu(const AccData& data);

    void start();
    void stop();
};

