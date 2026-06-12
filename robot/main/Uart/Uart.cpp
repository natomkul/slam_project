#include "Uart.hpp"

Uart::Uart(uint16_t TxPin, uint16_t RxPin, int baudRate, uint8_t* data, uint16_t dataSize)
    : data(data), dataSize(dataSize)
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

Uart::~Uart() = default;

void Uart::transmit()
{
}

int Uart::receive()
{
    size_t available = 0;
    uart_get_buffered_data_len(uart_num, &available);

    if (available > 0)
    {
        return uart_read_bytes(uart_num, data, available, 1000);
    }
    return 0;
}

Packet Uart::receiveDataToSend(){
    size_t available = 0;
    uart_get_buffered_data_len(uart_num, &available);

    if (available > 0) {
        length = uart_read_bytes(uart_num, data, available, 1000);
    } else {
        length = 0;
    }
    return Packet(data, length);
}
