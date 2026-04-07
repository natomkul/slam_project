#include "Pwm.hpp"

void Pwm::addChannel(gpio_num_t portNumber, uint8_t duty, uint32_t frequency){
    pwmChannels.insert({portNumber, PwmChannel(duty, frequency)});
};

