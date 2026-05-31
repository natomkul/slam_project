#include "SensorsNode.h"

SensorsNode::SensorsNode(TCPserver* server) 
    : Node("sensor_node"), server(server)
{
    scan_pub_ = this->create_publisher<sensor_msgs::msg::LaserScan>("/scan/raw", 10);
    imu_pub_ = this->create_publisher<sensor_msgs::msg::Imu>("/imu/raw", 10);

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

bool SensorsNode::proc()
{
    if (!server->connectSock())
    {
        return false;
    }

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
