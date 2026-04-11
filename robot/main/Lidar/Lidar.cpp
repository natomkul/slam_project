#include "Lidar.hpp"

void Lidar::startReceiving(){
    isReceiving = true;
    while(isReceiving){
        uart.receive();
        vTaskDelay(pdTICKS_TO_MS(sleepTime));
    }
}