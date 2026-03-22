#include "Leds.hpp"

void Leds::addLed(gpio_num_t portNumber, uint32_t frequency = DEFAULT_LED_BLINK_FREQUENCY){
    leds.insert({portNumber, Led(portNumber, frequency)});
}
