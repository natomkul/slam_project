#include "Encoder.hpp"
#include <cmath>
#include <driver/gpio.h>

#define PI 3.14

Encoder::Encoder(const uint16_t ppr, const gpio_num_t channelA, const gpio_num_t channelB, const float wheelRadius)
    : ppr(ppr), channelA(channelA), channelB(channelB), wheelRadius(wheelRadius)
{
    gpio_config_t io_conf{};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << channelA) | (1ULL << channelB);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);

    previousState = (gpio_get_level(channelA) << 1) | gpio_get_level(channelB);
    data[0] = 0x68;
}

Encoder::~Encoder() = default;

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
        pulseCount.fetch_add(1);
        lastEvent = clockwise;
        break;

    case 0b0010:
    case 0b0100:
    case 0b1101:
    case 0b1011:
        pulseCount.fetch_sub(1);
        lastEvent = counterclockwise;
        break;
    }

    previousState = currentState;
}

float Encoder::getRevolutions() const
{
    const float countsPerRevolution = static_cast<float>(ppr) * 210.0 * 4.0;
    return pulseCount.load() / countsPerRevolution;
}

float Encoder::getDistanceInMeters() const
{
    const float circumference = 2.0 * static_cast<float>(PI) * wheelRadius;
    return getRevolutions() * circumference;
}

void Encoder::reset()
{
    pulseCount.store(0);
}

int16_t Encoder::getPulseCount()
{
    return pulseCount.load();
}

int Encoder::receiveData()
{
    data[1] = pulseCount.load() & 0xFF;
    data[2] = (pulseCount.load() & 0xFF00) >> 8;
    data[3] = lastEvent;

    return sizeof(data);
}
