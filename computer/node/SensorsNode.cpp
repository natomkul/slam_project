#include "SensorsNode.h"

SensorsNode::SensorsNode(const char* PORT) 
    : Node("sensor_node"), server(new TCPserver(PORT))
{
    scan_pub_ = this->create_publisher<sensor_msgs::msg::LaserScan>("/scan", 10);

    running_ = false;  // thread jeszcze nie startuje

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
                std::cout << "Accel packet\n";

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
