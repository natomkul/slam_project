#pragma once

#include <cstdint>
#include <driver/uart.h>

/*
Class to communicate rx and tx via uart.
*/

class Uart{
    public:
    Uart(uint16_t TxPin, uint16_t RxPin, int baudRate, uint8_t* data, uint16_t dataSize, int& length) : data(data), dataSize(dataSize), length(length)
    {
        uart_config_t uart_config = {
            .baud_rate = baudRate,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .rx_flow_ctrl_thresh = 100,
            .source_clk = UART_SCLK_APB,
            .flags = {
                .allow_pd = false,
                .backup_before_sleep = false,
            }
        };

        ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
        ESP_ERROR_CHECK(uart_set_pin(uart_num, TxPin, RxPin, -1, -1));
        ESP_ERROR_CHECK(uart_driver_install(uart_num, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));
    }
    ~Uart() = default;
    
    void transmit();
    void receive();

    const int uart_buffer_size = (1024 * 2);


    uint8_t* data;
    uint16_t dataSize;

    int& length; // of read data

    QueueHandle_t uart_queue;
    const uart_port_t uart_num = UART_NUM_2;
};
