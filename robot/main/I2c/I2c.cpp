#include "i2c.hpp"
#include <memory>

void I2c::setUpMaster()
{
    i2c_master_bus_config_t busConfig{
        .i2c_port = i2cPort,
        .sda_io_num = sdaPortNumber,
        .scl_io_num = sclPortNumber,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 5,
        .intr_priority = 0,
        .trans_queue_depth = 4,
        .flags = {
            .enable_internal_pullup = true,
            .allow_pd = false},
    };

    esp_ret = i2c_new_master_bus(&busConfig, &bus);
    if (esp_ret != ESP_OK)
    {
        printf("Error: Failed to initialize bus, ERROR: 0x%x\r\n", esp_ret);
        abort();
    }
}

[[nodiscard]] std::shared_ptr<I2cSlave> I2c::createSlave(uint8_t reg)
{
    I2cSlave slave(reg);

    esp_ret = i2c_master_bus_add_device(bus, &slave.devConfig, &slave.device);
    if (esp_ret != ESP_OK)
    {
        printf("Error: Failed to initialize device\r\n");
        abort();
    }
    auto sharedPtr = std::make_shared<I2cSlave>(slave);
    slaves.push_back(sharedPtr);
    return sharedPtr;
}

void I2c::transmitReceive(std::shared_ptr<I2cSlave> slave, uint8_t reg, uint8_t *buffer, uint8_t writeSize)
{
    esp_ret = i2c_master_transmit_receive(slave->device, &reg, 1, buffer, 20, DEFAULT_TIMEOUT);
    if (esp_ret != ESP_OK)
    {
        printf("Error: Failed to read from the device\r\n");
    }
}

void I2c::transmit(std::shared_ptr<I2cSlave> slave, uint8_t reg, uint8_t *cmd, uint8_t writeSize)
{
    esp_ret = i2c_master_transmit(slave->device, cmd, writeSize, DEFAULT_TIMEOUT);
    if (esp_ret != ESP_OK)
    {
        printf("Error: Failed to read from the device\r\n");
    }
}
