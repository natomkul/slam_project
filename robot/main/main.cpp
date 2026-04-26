
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
    const uint32_t sleep_time = 200;


void app_main()
{
    
    vTaskDelay(sleep_time * 20/portTICK_PERIOD_MS); 
    printf("\nstart\n");

    static I2c i2c{0, 0, GPIO_NUM_8, GPIO_NUM_9};
    static Accelerometer accelerometer{i2c};
    static Lidar lidar{11, 12, 230400};
    static DataExchanger dataExchanger("192.168.88.38", 3000, "WZnet_BUR", "piwc4321");
    
    dataExchanger.appendToSending(std::bind(&Lidar::receiveData, &lidar), lidar.data);

    vTaskDelay(sleep_time * 20/portTICK_PERIOD_MS); 

    printf("start tcpClient");
    dataExchanger.startTcpClient();

    while(true){
        printf("before lidar grab data\n");
        // int length = lidar.receiveData();
        // printf("length = %d\n", length);
        // for(int i = 0; i < length; i++){
        //     printf("%x ", lidar.data[i]);
        // }
        printf("\n");
        vTaskDelay(sleep_time * 10/portTICK_PERIOD_MS);    
    };
};
