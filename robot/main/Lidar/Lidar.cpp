#include "Lidar.hpp"


int Lidar::receiveData(){
    return uart.receive();
}