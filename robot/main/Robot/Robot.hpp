#pragma once

#include "Accelerometer/Accelerometer.hpp"
#include "DataExchanger/DataExchanger.hpp"
#include "Led/Leds.hpp"
#include "Lidar/Lidar.hpp"
#include "Motor/MotorManager.hpp"

enum State
{
    stopped,
    connected,
    running,
    crashed
};

/*
Class of robot itself.
*/
class Robot
{
public:
    Robot(Accelerometer accelerometer, Leds leds, Lidar lidar, MotorManager motors);
    ~Robot();

private:
    Accelerometer accelerometer;
    Leds leds;
    Lidar lidar;
    MotorManager motors;
    State state = stopped;

    void start();
    void stop();
};
