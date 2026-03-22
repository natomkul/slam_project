#pragma once

#include <cstdint>

/*
Class to communicate rx and tx via uart.
*/

class Uart{
    public:
    Uart(uint32_t registerValue, uint32_t baudRate) :
    registerValue(registerValue),
    baudRate(baudRate){}
    ~Uart() = default;
    
    void receive();

    private:
    const volatile uint32_t registerValue;
    const uint32_t baudRate;
};
