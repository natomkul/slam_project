#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "Robot/Robot.hpp"
#include <chrono>
#include <thread>
#include "driver/i2c_master.h"
#include <thread>
#include <functional>

extern "C" {
    void app_main(void);
}
    const uint32_t sleep_time = 200;


void app_main()
{
    // // Leds leds

    // // Motor

    // I2c i2c{0, GPIO_NUM_8, GPIO_NUM_9};
    // Accelerometer accelerometer{i2c, 0x68, 0x04, 0x7E};
    
    // Lidar lidar{11, 12, 230400};

    // // Robot robot{Accelerometer{i2c, 0x68, 0x04, 0x7E}, Leds{}, Lidar{11, 12, 230400}, Motor{}, DataExchanger{}};

    // std::thread t1(&Accelerometer::startMeasuring, accelerometer);
    // std::thread t2(&Lidar::startReceiving, lidar);

    // uint length;
    // while(true){
    //     printf("measurement start\n");
    //     length = lidar.length;
    //     for (int i = 0; i < length; i++) {
    //         printf("%02X ", lidar.data[i]);
    //     }
    //     printf("\n");
    //     printf("measurement end\n");
    //     vTaskDelay(sleep_time * 10/portTICK_PERIOD_MS); 
    // }
    Lidar lidar{11, 12, 230400};
    DataExchanger dataExchanger("192.168.1.1", 3000, "your ssid", "your password");
    dataExchanger.getLidarStartReceiveDataMethod(std::bind(&Lidar::receiveData, &lidar));
    dataExchanger.lidarReceiveData();
    dataExchanger.startTcpClient();
};
