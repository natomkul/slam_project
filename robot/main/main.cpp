
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
#include "DataExchanger/DataExchanger.hpp"


extern "C" {
    void app_main(void);
}


void app_main()
{
    
    vTaskDelay(5000/portTICK_PERIOD_MS); 
    printf("\n---initialize device---\n");

    static I2c i2c{0, 0, GPIO_NUM_8, GPIO_NUM_9};
    static Accelerometer accelerometer{i2c};
    static Lidar lidar{11, 12, 230400};
    static DataExchanger dataExchanger("10.170.219.13", 3000, "realme 9 Pro+", "c4i39885");
    
    dataExchanger.appendToSending(std::bind(&Lidar::receiveData, &lidar), lidar.data);
    dataExchanger.appendToSending(std::bind(&Accelerometer::receiveData, &accelerometer), accelerometer.data);

    vTaskDelay(5000/portTICK_PERIOD_MS); 

    printf("\n---start tcp client---\n");

    dataExchanger.startTcpClient();

    while(true){
        printf("---\n");
        vTaskDelay(5000/portTICK_PERIOD_MS);  
    };
};
