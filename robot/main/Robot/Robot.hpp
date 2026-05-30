#pragma once

#include "Accelerometer/Accelerometer.hpp"
#include "DataExchanger/DataExchanger.hpp"
#include "Led/Leds.hpp"
#include "Lidar/Lidar.hpp"
#include "Motor/MotorManager.hpp"
#include "State.hpp"

/*
Class of robot itself.
*/
class Robot
{
public:
    Robot(Accelerometer accelerometer, Leds leds, Lidar lidar, MotorManager motors) : accelerometer(accelerometer),
                                                                                      leds(leds),
                                                                                      lidar(lidar),
                                                                                      motors(motors)
    {
        start();
    }
    ~Robot()
    {
        stop();
    };

private:
    Accelerometer accelerometer;
    Leds leds;
    Lidar lidar;
    MotorManager motors;
    State state = stopped;

    void start();
    void stop();
};
