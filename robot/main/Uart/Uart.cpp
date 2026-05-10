#include "Uart.hpp"

void Uart::transmit(){

}

int Uart::receive(){
    size_t available = 0;
    uart_get_buffered_data_len(uart_num, &available);

    if (available > 0) {
        return uart_read_bytes(uart_num, data, available, 1000);
    }
    return 0;
}
