
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/i2c_master.h"

#include <chrono>
#include <thread>
#include <stdio.h>

#include "Robot/Robot.hpp"
#include "Motor/Motors.hpp"
#include "Pwm/Pwm.hpp"

extern "C" {
    void app_main(void);
}
    const uint32_t sleep_time = 200;


void app_main()
{
    
    vTaskDelay(sleep_time * 10/portTICK_PERIOD_MS); 
    printf("\nstart\n");
    // I2c i2c{0, 0, GPIO_NUM_8, GPIO_NUM_9};
    Pwm pwm{5000};

    // Accelerometer accelerometer{i2c};
    Leds leds{pwm};
    // Lidar lidar{11, 12, 230400};
    // Motors motors;

    // Robot robot{accelerometer, leds, lidar, motors};



    leds.addLed(GPIO_NUM_6);
    leds[GPIO_NUM_6].on();
    leds.addLed(GPIO_NUM_7);
    leds[GPIO_NUM_7].blinkOn();


    while(true){
        printf("smth\n");
        vTaskDelay(sleep_time * 10/portTICK_PERIOD_MS); 
    }
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
