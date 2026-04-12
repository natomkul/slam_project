#include "DataExchanger.hpp"

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_mac.h"
#include "esp_eth.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "esp_http_client.h"
#include "esp_event.h"
#include "esp_system.h"

#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include "nvs_flash.h"
#include "ping/ping_sock.h"
#include "driver/gpio.h"

static const char *TAG = "TCP SOCKET Client";
QueueHandle_t sendQueue;

EventGroupHandle_t wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0

DataExchanger::DataExchanger(std::string serverIP, int port, std::string ssid, std::string password) 
    : serverIP(serverIP), port(port), ssid(ssid), password(password)
{
    sendQueue = xQueueCreate(10, sizeof(Packet));
    wifiConnection();
};

void DataExchanger::getLidarStartReceiveDataMethod(std::function<Packet()> method) {
    lidarReceiveData = method;
}

void DataExchanger::wifiEventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data){
    switch (event_id)
    {
    case WIFI_EVENT_STA_CONNECTED:
        ESP_LOGI(TAG, "WiFi connected");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        ESP_LOGW(TAG, "WiFi disconnected, retrying...");
        esp_wifi_connect();
        break;
    case IP_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_LOGI(TAG, "Got server IP, ready");
        break;
    default:
        break;
    }
}

void DataExchanger::wifiConnection(){
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifiEventHandler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifiEventHandler, NULL);

    wifi_config_t wifi_configuration = {};
    strncpy((char*)wifi_configuration.sta.ssid,
        ssid.c_str(), sizeof(wifi_configuration.sta.ssid));
    strncpy((char*)wifi_configuration.sta.password,
        password.c_str(), sizeof(wifi_configuration.sta.password));

    wifi_configuration.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    esp_wifi_set_config(WIFI_IF_STA, &wifi_configuration);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_connect();
}

int DataExchanger::createSocketAndConnect()
{
    struct sockaddr_in dest_addr{};
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, serverIP.c_str(), &dest_addr.sin_addr) <= 0)
    {
        ESP_LOGE(TAG, "Invalid IP");
        return -1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0)
    {
        ESP_LOGE(TAG, "Socket creation failed: errno %d", errno);
        return -1;
    }

    int flag = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

    timeval timeout{};
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    ESP_LOGI(TAG, "Connecting to %s:%d", serverIP.c_str(), port);

    if (connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) != 0)
    {
        ESP_LOGW(TAG, "Connect failed: errno %d", errno);
        close(sock);
        return -1;
    }

    ESP_LOGI(TAG, "Connected!");
    return sock;
}

void DataExchanger::tcpSendTask(void* arg)
{
    auto* self = static_cast<DataExchanger*>(arg);
    int sock = self->sock;

    while (true)
    {
        Packet packet = self->lidarReceiveData();
        int totalLen = packet.length + 3;

        int sent = send(sock, packet.packedData, totalLen, 0);
        if (sent < 0)
        {
            ESP_LOGE("TCP", "Send failed: errno %d", errno);
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }

    vTaskDelete(NULL);
}

void DataExchanger::tcpRecieveTask(void* arg){
    //
}

void DataExchanger::startTcpClient()
{
    while (true)
    {
        sock = createSocketAndConnect();

        if (sock < 0)
        {
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        socketAlive = true;
        ESP_LOGI(TAG, "Socket ready: %d", sock);

        TaskHandle_t sendTaskHandle = nullptr;
        //TaskHandle_t recvTaskHandle = nullptr;

        xTaskCreate(tcpSendTask, "tcpSend", 4096, this, 5, &sendTaskHandle);
        //xTaskCreate(tcpRecieveTask, "tcpRecv", 4096, (void*)sock, 5, &recvTaskHandle);

        while (socketAlive)
        {
            vTaskDelay(pdMS_TO_TICKS(500));
        }

        ESP_LOGW(TAG, "Connection lost.");

        close(sock);
        if (sendTaskHandle) vTaskDelete(sendTaskHandle);
        //if (recvTaskHandle) vTaskDelete(recvTaskHandle);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}