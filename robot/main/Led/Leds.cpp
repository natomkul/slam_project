#include "Leds.hpp"

void Leds::addLed(gpio_num_t portNumber){
    leds.insert({portNumber, Led(portNumber)});
}
