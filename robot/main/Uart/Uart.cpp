#include "Uart.hpp"

void Uart::transmit(){

}

void Uart::receive(){
    size_t available = 0;
    uart_get_buffered_data_len(uart_num, &available);

    if (available > 0) {
        length = uart_read_bytes(uart_num, data, available, 1000);
    } else {
        length = 0;
    }
}

Packet Uart::receiveDataToSend(){
    size_t available = 0;
    uart_get_buffered_data_len(uart_num, &available);

    if (available > 0) {
        length = uart_read_bytes(uart_num, data, available, 1000);
    } else {
        length = 0;
    }
    return Packet(DataType::LIDAR, length, data);
}
