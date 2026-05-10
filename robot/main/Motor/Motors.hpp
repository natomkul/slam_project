#pragma once

#include <map>
#include "driver/gpio.h"
#include "Motor/Motor.hpp"
#include "Motor/Encoder.hpp"

class Motors{
    public:
    Motors() = default;
    ~Motors() = default;

    Motor operator[](gpio_num_t portNumber){
        return motors.at(portNumber);
    }
    
    void addMotor(const gpio_num_t portNumber, const Encoder encoder,  const uint16_t wheelDiameterInMM);
    
    private:
    std::map<gpio_num_t, Motor> motors;
};
