#include "PwmChannel.hpp"

ledc_channel_t PwmChannel::reserveNextChannel()
{
    static int nextChannel = static_cast<int>(LEDC_CHANNEL_0);
    if (nextChannel > static_cast<int>(LEDC_CHANNEL_7))
    {
        printf("Error: No LEDC channels left\r\n");
        abort();
    }

    ledc_channel_t reserved = static_cast<ledc_channel_t>(nextChannel);
    nextChannel++;
    return reserved;
}

void PwmChannel::changeDuty(uint8_t duty)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel);
}

void PwmChannel::changeFrequency(uint32_t frequency)
{
    ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, frequency);
}