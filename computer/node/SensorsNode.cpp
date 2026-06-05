#include "SensorsNode.h"

SensorsNode::SensorsNode(std::shared_ptr<TCPserver> server) 
    : Node("sensor_node"), server(server)
{
    scan_pub_ = this->create_publisher<sensor_msgs::msg::LaserScan>("/scan", 10);
    imu_pub_ = this->create_publisher<sensor_msgs::msg::Imu>("/imu", 10);
    odom_pub_ = this->create_publisher<nav_msgs::msg::Odometry>("/odom", 10);
    
    running_ = false;

    RCLCPP_INFO(this->get_logger(), "SensorsNode initialized");
}

sensor_msgs::msg::LaserScan SensorsNode::LidarDataToScan(const LiData &data)
{
    sensor_msgs::msg::LaserScan scan;

    scan.header.frame_id = "lidar_link";
    scan.header.stamp = this->get_clock()->now();;

    float start = ((float) data.startAngle) * M_PI / 180.0f;
    float end = ((float) data.endAngle) * M_PI / 180.0f;

    scan.angle_min = start;
    scan.angle_max = end;

    scan.angle_increment = (end - start) / (POINT_PER_PACK - 1);

    scan.scan_time = (float) data.timestamp; //- "prev timestamp" ;

    //dokładnosc/ blad pomiarowy Lidara
    scan.range_min = 0.02f;
    scan.range_max = 12.0f;

    scan.ranges.resize(POINT_PER_PACK);
    scan.intensities.resize(POINT_PER_PACK);

    for (int i = 0; i < POINT_PER_PACK; i++)
    {
        scan.ranges[i] = ((float) data.point[i].distanceValue) / 1000.0;
        scan.intensities[i] = ((float) data.point[i].confidence);
    }

    return scan;
}

void SensorsNode::publish_scan(const LiData& data)
{
    auto msg = LidarDataToScan(data);
    scan_pub_->publish(msg);
}

sensor_msgs::msg::Imu SensorsNode::AccelDataToImu(const AccData &data)
{
    sensor_msgs::msg::Imu imu_msg;

    imu_msg.header.frame_id = "imu_link";
    imu_msg.header.stamp = this->get_clock()->now();;

    imu_msg.angular_velocity.x = data.gx;
    imu_msg.angular_velocity.y = data.gy;
    imu_msg.angular_velocity.z = data.gz;

    imu_msg.linear_acceleration.x = data.ax;
    imu_msg.linear_acceleration.y = data.ay;
    imu_msg.linear_acceleration.z = data.az;

    imu_msg.orientation_covariance[0] = -1;

    return imu_msg;
}

void SensorsNode::publish_imu(const AccData& data)
{
    auto msg = AccelDataToImu(data);
    imu_pub_->publish(msg);
}

nav_msgs::msg::Odometry SensorsNode::EnDataToOdom(const EnPair &data)
{
    nav_msgs::msg::Odometry odom_msg;
    rclcpp::Time current_time = this->get_clock()->now();

    odom_msg.header.frame_id = "odom";
    odom_msg.child_frame_id = "odom_link";
    odom_msg.header.stamp = current_time;

    float d_center = (data.delta_right + data.delta_left) / 2.0;
    float d_theta = (data.delta_right - data.delta_left) / WHEEL_TRACK;

    x_pos_ += d_center * std::cos(theta_ + d_theta / 2.0);
    y_pos_ += d_center * std::sin(theta_ + d_theta / 2.0);
    theta_ += d_theta;

    static rclcpp::Time last_time = current_time;
    float dt = (current_time - last_time).seconds();

    if (dt > 0.0) {
        odom_msg.twist.twist.linear.x = d_center / dt;
        odom_msg.twist.twist.angular.z = d_theta / dt;
    }

    last_time = current_time;

    odom_msg.pose.pose.position.x = x_pos_;
    odom_msg.pose.pose.position.y = y_pos_;
    odom_msg.pose.pose.position.z = 0.0;

    odom_msg.pose.pose.orientation.x = 0.0;
    odom_msg.pose.pose.orientation.y = 0.0;
    odom_msg.pose.pose.orientation.z = std::sin(theta_ / 2.0);
    odom_msg.pose.pose.orientation.w = std::cos(theta_ / 2.0);

    return odom_msg;
}

void SensorsNode::publish_odom(const EnData& raw_data)
{
    if (raw_data.LorR == 1) 
    {
        para.delta_left = raw_data.meters;
        left_received_ = true;
    }
    else if (raw_data.LorR == 2) 
    {
        para.delta_right = raw_data.meters;
        right_received_ = true;
    }

    if (left_received_ && right_received_) 
    {
        left_received_ = false;
        right_received_ = false;
    
        auto msg = EnDataToOdom(para);
        odom_pub_->publish(msg);
    }
}
    

bool SensorsNode::proc()
{
    bool run = true;

    while(run) 
    {
        auto res = server->receiveData();

        if (!res)
        {
            run = false;
            break;
        }

        std::visit([this](auto&& v)
        {
            using T = std::decay_t<decltype(v)>;

            if constexpr (std::is_same_v<T, std::monostate>)
            {
                return;
            }
            else if constexpr (std::is_same_v<T, LiData>)
            {
                publish_scan(v);
            }

            else if constexpr (std::is_same_v<T, AccData>)
            {
                publish_imu(v);
            }
            else if constexpr (std::is_same_v<T, EnData>)
            {
                publish_odom(v);
            }

        }, *res);
    
    }

    return false;
}

void SensorsNode::start()
{
    running_ = true;

    proc_thread_ = std::thread([this]()
    {
        this->proc();
    });
}

void SensorsNode::stop()
{
    running_ = false;

    if (proc_thread_.joinable())
        proc_thread_.join();
}
