#include "Led/Led.hpp"

#include <thread>
#include <chrono>

void Led::on(){
    gpio_set_level(portNumber, HIGH);
    // if(gpio_set_level(portNumber, HIGH) != 0){
    //     printf("Error!\n");
    // }
}

void Led::off(){
    gpio_set_level(portNumber, LOW);
    // if(gpio_set_level(portNumber, LOW) != 0){
    //      printf("Error!\n");
    // }
}

void Led::blinkOn(){
    on();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000/blinkFrequency));
    off();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000/blinkFrequency));
}

void Led::blinkOff(){

}
