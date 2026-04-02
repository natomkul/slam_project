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
    const i2c_port_num_t i2c_port = 0;
    const gpio_num_t i2c_sda_pin = GPIO_NUM_8;
    const gpio_num_t i2c_scl_pin = GPIO_NUM_9;
    const uint8_t i2c_glich_ignore_cnt = 3;
    const uint16_t bm160_addr = 0x68;
    const uint32_t scl_speed = 100000;
    const uint32_t sleep_time = 200;

    const uint16_t dataRegister = 0x04;

void app_main()
{
    vTaskDelay(sleep_time * 10/portTICK_PERIOD_MS);
    printf("start\n");
    I2c i2c{0, GPIO_NUM_8, GPIO_NUM_9};
    Accelerometer accl{i2c, 0x68, 0x04, 0x7E};
    accl.calibrate();
    while(true){
        accl.getMeasurement();
        
        printf("mag: %d, %d, %d\nrhall: %d\ngyr: %d, %d, %d\n",
            accl.data.magnitudeX,
            accl.data.magnitudeY,
            accl.data.magnitudeZ,
            accl.data.rhall,
            accl.data.gyroscopeX,
            accl.data.gyroscopeY,
            accl.data.gyroscopeZ
        );
        vTaskDelay(sleep_time/portTICK_PERIOD_MS);
    }
    
    // Robot robot{Accelerometer{i2c, 0x68, 0x04, 0x7E}, Leds{}, Lidar{1, 1}, Motor{}, DataExchanger{}};

}



/*

    printf("Hello!\n");

    // Robot robot{Accelerometer{}, Leds{}, Lidar{1, 1}, Motor{}};
    gpio_num_t redLedPortNumber = GPIO_NUM_5;
    gpio_num_t yellowLedPortNumber = GPIO_NUM_6;
    gpio_num_t greenLedPortNumber = GPIO_NUM_7;
    Leds leds;
    leds.addLed(redLedPortNumber, 2);
    leds.addLed(yellowLedPortNumber, 2);
    leds.addLed(greenLedPortNumber, 2);

    int t = 1000;

    while(t-- > 0){
        leds[redLedPortNumber].on();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        leds[redLedPortNumber].off();
        leds[yellowLedPortNumber].on();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        leds[yellowLedPortNumber].off();
        leds[greenLedPortNumber].on();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        leds[greenLedPortNumber].off();
    }
        
    */