#include "Lidar.hpp"

Packet Lidar::receiveData(){
    return uart.receiveDataToSend();
}