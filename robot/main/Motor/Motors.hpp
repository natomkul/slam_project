#pragma once

#include "Motor.hpp"
#include <map>
#include "driver/gpio.h"

class Motors{
    Motors() = default;
    ~Motors() = default;

    Motor operator[](gpio_num_t portNumber){
        return motors.at(portNumber);
    }

    void addMotor(gpio_num_t);
    
    private:
    std::map<gpio_num_t, Motor> motors;
};
