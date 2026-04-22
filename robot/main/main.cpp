
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/i2c_master.h"

#include <chrono>
#include <thread>
#include <stdio.h>

#include "Robot/Robot.hpp"
#include "Motor/Motors.hpp"

extern "C" {
    void app_main(void);
}
    const uint32_t sleep_time = 200;


void app_main()
{
    I2c i2c{0, 0, GPIO_NUM_8, GPIO_NUM_9};
    Accelerometer accelerometer{i2c};
    Leds leds{};
    Lidar lidar{11, 12, 230400};
    Motors motors;

    Robot robot{accelerometer, leds, lidar, motors};

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
};
