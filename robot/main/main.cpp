#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "Robot/Robot.hpp"
#include <chrono>
#include <thread>
#include "driver/i2c_master.h"

extern "C" {
    void app_main(void);
}
    const uint32_t sleep_time = 200;


void app_main()
{
    vTaskDelay(sleep_time * 10/portTICK_PERIOD_MS);
    printf("start\n");

    I2c i2c{0, GPIO_NUM_8, GPIO_NUM_9};
    // Leds leds
    Lidar lidar{11, 12, 116800};
    // Motor

    // Robot robot{Accelerometer{i2c, 0x68, 0x04, 0x7E}, Leds{}, Lidar{1, 1}, Motor{}, DataExchanger{}};
};
