#include "Pwm.hpp"

void Pwm::addChannel(gpio_num_t portNumber, uint8_t duty){
    if (pwmChannels.find(portNumber) != pwmChannels.end()){
        return;
    }
    pwmChannels.insert({portNumber, PwmChannel(portNumber, duty)});
};

