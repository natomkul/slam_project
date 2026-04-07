#include "Uart.hpp"

void Uart::transmit(){

}

void Uart::receive(){
    length = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t*)&length));
    length = uart_read_bytes(uart_num, data, length, 100);
}
