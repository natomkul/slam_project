#pragma once

#include "Accelerometer/Accelerometer.hpp"
#include "Led/Leds.hpp"
#include "Lidar/Lidar.hpp"
#include "Motor/Motor.hpp"
#include "State.hpp"

/*
Class of robot itself.
*/
class Robot {
    public:
    Robot(Accelerometer accelerometer, Leds leds, Lidar lidar, Motor motor) :
        accelerometer(accelerometer),
        leds(leds),
        lidar(lidar),
        motor(motor) {}
    ~Robot() = default;

    private:
    Accelerometer accelerometer;
    Leds leds;
    Lidar lidar;
    Motor motor;
    State state;
};
