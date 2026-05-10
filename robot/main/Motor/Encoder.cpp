#include "Encoder.hpp"
#include <cmath>

#define PI 3.14

void Encoder::update()
{
    uint8_t currentState = (gpio_get_level(channelA) << 1) | gpio_get_level(channelB);

    if (currentState == previousState)
    {
        return;
    }

    uint8_t state = (previousState << 2) | currentState;
    switch (state)
    {
        case 0b0001:
        case 0b0111:
        case 0b1110:
        case 0b1000:
            pulseCount++;
            lastEvent = clockwise;
            break;

        case 0b0010:
        case 0b0100:
        case 0b1101:
        case 0b1011:
            pulseCount--;
            lastEvent = counterclockwise;
            break;
    }

    previousState = currentState;
}

float Encoder::getRevolutions() const
{
    const float countsPerRevolution = static_cast<float>(ppr) * 210.0 * 4.0;
    return pulseCount / countsPerRevolution;
}

float Encoder::getDistanceInMeters() const
{
    const float circumference = 2.0 * static_cast<float>(PI) * wheelRadius;
    return getRevolutions() * circumference;
}

// float Encoder::getVelocityInMpS()
// {
//     int64_t currentTime = esp_timer_get_time();
//     int64_t timeDifference = currentTime - lastVelocityTime;

//     if (timeDifference <= 0)
//         return 0.0f;

//     int32_t currentCount = pulseCount;
//     int32_t countDifference = currentCount - lastVelocityCount;

//     const float countsPerRevolution = static_cast<float>(ppr) * 4.0f;
//     const float revolutionDifference = countDifference / countsPerRevolution;

//     const float circumference = 2.0f * static_cast<float>(M_PI) * wheelRadius;
//     const float distanceDifference = revolutionDifference * circumference;

//     const float timeDifferenceInSeconds = timeDifference / 1'000'000.0f;

//     lastVelocityTime = currentTime;
//     lastVelocityCount = currentCount;

//     return distanceDifference / timeDifferenceInSeconds;
// }

void Encoder::reset()
{
    pulseCount = 0;
}

int Encoder::receiveData(){
    data[1]= pulseCount & 0xFF;
    data[2]= (pulseCount & 0xFF00) >> 8; 
    data[3] = lastEvent;

    return sideof(data);
}