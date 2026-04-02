#pragma once

#include <driver/i2c_master.h>
#include <vector>
#include "i2cSlave.hpp"
#include <memory>

#define DEFAULT_TIMEOUT 2000

class I2c{
    public:
    I2c(uint16_t address, gpio_num_t sdaPortNumber, gpio_num_t sclPortNumber) :
    address(address), sdaPortNumber(sdaPortNumber), sclPortNumber(sclPortNumber) {
        setUpMaster();
    };
    ~I2c() = default;
    
    void setUpMaster();
    std::shared_ptr<I2cSlave>  createSlave(uint8_t reg);
    void transmit(std::shared_ptr<I2cSlave> slave, uint8_t reg, uint8_t* cmd, uint8_t writeSize);
    void transmitReceive(std::shared_ptr<I2cSlave> slave, uint8_t reg, uint8_t* buffer, uint8_t writeSize);

    private:
    const uint16_t address;
    std::vector<std::shared_ptr<I2cSlave>> slaves;
    esp_err_t esp_ret;
    i2c_master_bus_handle_t bus;

    const i2c_port_num_t i2cPort = 0;
    const gpio_num_t sdaPortNumber;
    const gpio_num_t sclPortNumber;
    const uint8_t glitchIgnoreCount = 5;
};
