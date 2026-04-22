#pragma once

#include "Led.hpp"
#include <map>

/*
Stores multiple LEDs inside and allows [] access to them.
*/

class Leds{
    public:
    Leds(){
        
    };
    ~Leds() = default;

    Led& operator[](gpio_num_t portNumber){
        return leds.at(portNumber);
    }

    void addLed(gpio_num_t portNumber);

    private:
    std::map<gpio_num_t, Led> leds;
};
