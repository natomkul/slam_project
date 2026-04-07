#pragma once

#include <cstdint>
#include "driver/ledc.h"
#include "driver/gpio.h"


//  int gpio_num;                   /*!< the LEDC output gpio_num, if you want to use gpio16, gpio_num = 16 */
//     ledc_mode_t speed_mode;         /*!< LEDC speed speed_mode, high-speed mode (only exists on esp32) or low-speed mode */
//     ledc_channel_t channel;         /*!< LEDC channel (0 - LEDC_CHANNEL_MAX-1) */
//     ledc_intr_type_t intr_type __attribute__((deprecated));     /*!< @deprecated, no need to explicitly configure interrupt, handled in the driver */
//     ledc_timer_t timer_sel;         /*!< Select the timer source of channel (0 - LEDC_TIMER_MAX-1) */
//     uint32_t duty;                  /*!< LEDC channel duty, the range of duty setting is [0, (2**duty_resolution)] */
//     int hpoint;                     /*!< LEDC channel hpoint value, the range is [0, (2**duty_resolution)-1] */
//     ledc_sleep_mode_t sleep_mode;   /*!< choose the desired behavior for the LEDC channel in Light-sleep */
//     /// Extra configuration flags for LEDC channel
//     struct ledc_channel_flags {
//         unsigned int output_invert: 1; /*!< Enable (1) or disable (0) gpio output invert */
//     } flags;                        /*!< Extra configuration flags for LEDC channel */
//     bool deconfigure;               /*!< Set this field to de-configure a LEDC channel which has been configured before
//                                          The 


class PwmChannel{
    public:
    PwmChannel(gpio_num_t portNumber, uint8_t duty, uint32_t frequency){
        ledc_channel_config_t config = {
            .gpio_num = portNumber,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = LEDC_CHANNEL_0,
            .intr_type = LEDC_INTR_DISABLE,
            .timer_sel = LEDC_TIMER_0,
            .duty = duty,
            .hpoint = 0,
            .sleep_mode = LEDC_SLEEP_MODE_KEEP_ALIVE,
            .flags = {
                .output_invert = false,
            },
            .deconfigure = true,
        };
        ledc_channel_config(&config);
    }
    ~PwmChannel() = default;

    void changeDuty();
    void changeFrequency();

    private:
        uint8_t duty;
        uint32_t frequency;
};
