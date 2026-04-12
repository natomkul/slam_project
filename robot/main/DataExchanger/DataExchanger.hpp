#pragma once

#include <cstdint>
#include <string>
#include <functional>
#include "esp_event_base.h"
#include "Lidar/Lidar.hpp"
#include "Packet.hpp"

class DataExchanger {
public:
    DataExchanger(std::string serverIP, int port, std::string ssid, std::string password);
    ~DataExchanger() = default;

    void getLidarReceiveDataMethod(std::function<Packet()> method);
    void getAccelerometerReceiveDataMethod(std::function<Packet()> method);
    std::function<Packet()> lidarReceiveData;
    std::function<Packet()> accelerometerReceiveData;

    static void wifiEventHandler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
    void wifiConnection();

    void lidarTask(void* arg);

    static void tcpSendLidarDataTask(void* arg);
    static void tcpSendAccelerometerDataTask(void* arg);
    static void tcpRecieveTask(void* arg);
    int createSocketAndConnect();
    void startTcpClient();

    
private:
    char rx_buffer[128];

    int sock;
    std::string serverIP;
    int port;
    std::string ssid;
    std::string password;
    bool socketAlive{false};
};
