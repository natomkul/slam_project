#pragma once

#include <cstdint>
#include <driver/gpio.h>

#include "Pwm/Pwm.hpp"


/*
Class providing usage of motors via PWM. Should calculate
speed based on wheel diameter and PWM given or smth. More data better.

*/
class Motor {   
    Motor(uint16_t diameterInMM, uint16_t rpm, gpio_num_t portNumber){};
    
};
