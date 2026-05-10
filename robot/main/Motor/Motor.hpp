#pragma once

#include <cstdint>
#include <driver/gpio.h>
#include "Motor/Encoder.hpp"
#include "Pwm/Pwm.hpp"


/*
Class providing usage of single motor via PWM. Should calculate
ticks needed based on wheel diameter and allow to move by certain distance.

*/
class Motor {   
    public:
    Motor(const gpio_num_t portNumber, const Encoder encoder,  const uint16_t wheelDiameterInMM, Pwm& pwm)
    : portNumber(portNumber), encoder(encoder), wheelDiameterInMM(wheelDiameterInMM), pwm(pwm){
        pwm.addChannel(portNumber, 0);
    };
    ~Motor(){};
    
    private:
    const gpio_num_t portNumber;
    const Encoder encoder;
    const uint16_t wheelDiameterInMM; // to estimate distance
    Pwm& pwm;
};
