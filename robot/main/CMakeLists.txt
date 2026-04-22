idf_component_register(SRCS 
    "Accelerometer/Accelerometer.cpp"
    "DataExchanger/DataExchanger.cpp" 
    "I2c/I2c.cpp"
    "I2c/I2cSlave.cpp"
    "Led/Led.cpp"
    "Led/Leds.cpp"
    "Lidar/Lidar.cpp"
    "Motor/Motor.cpp"
    "Pwm/Pwm.cpp"
    "Pwm/PwmChannel.cpp"
    "Robot/Robot.cpp"
    "Uart/Uart.cpp"
    "main.cpp"
    REQUIRES esp_driver_uart
    REQUIRES esp_driver_i2c
    REQUIRES esp_driver_gpio
    REQUIRES esp_driver_ledc
    INCLUDE_DIRS ".")
