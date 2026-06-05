#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "TCPserver.h"
#include <thread>
#include <atomic>

struct EnPair 
{
    float delta_left = 0.0f;
    float delta_right = 0.0f;
};

class SensorsNode : public rclcpp::Node
{
 private:
    rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr scan_pub_;
    sensor_msgs::msg::LaserScan LidarDataToScan(const LiData &data);

    rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr imu_pub_;
    sensor_msgs::msg::Imu AccelDataToImu(const AccData &data);

    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub_;
    nav_msgs::msg::Odometry EnDataToOdom(const EnPair &data);

    std::shared_ptr<TCPserver> server;
    
    std::thread proc_thread_;
    std::atomic<bool> running_{false};

    bool proc();

    static constexpr float WHEEL_TRACK = 0.25; // Odległość między kołami w metrach

    double x_pos_ = 0.0;
    double y_pos_ = 0.0;
    double theta_ = 0.0;

    EnPair para; 
    bool left_received_ = false;
    bool right_received_ = false;

 public:
    SensorsNode(std::shared_ptr<TCPserver> server);

    void publish_scan(const LiData& data);
    void publish_imu(const AccData& data);
    void publish_odom(const EnData& raw_data);
    
    void start();
    void stop();
};

