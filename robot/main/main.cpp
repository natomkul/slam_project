#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "Robot/Robot.hpp"
#include <chrono>
#include <thread>

extern "C" {
    void app_main(void);
}

void app_main()
{
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
}
