#pragma once

#include <cstdint>

class Encoder{
    // 7ppr
    // add connecting to a pin that sends encoder information
    Encoder(uint16_t ppr, uint16_t wheelDiameterInMM): ppr(ppr), wheelDiameterInMM(wheelDiameterInMM){

    }
    ~Encoder(){

    };

    void trackDistance();

    int64_t distanceTravelledInMM{0};
    
    private:
    const uint16_t ppr; // pulses per revolution
    const uint16_t wheelDiameterInMM;
};
