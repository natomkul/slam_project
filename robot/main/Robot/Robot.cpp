#include "Robot/Robot.hpp"

Robot::Robot(Accelerometer accelerometer, Leds leds, Lidar lidar, MotorManager motors) : accelerometer(accelerometer),
                                                                                        leds(leds),
                                                                                        lidar(lidar),
                                                                                        motors(motors)
{
    start();
}

Robot::~Robot()
{
    stop();
}

void Robot::start() {
    // accl start
    // lidar start
    // encoders start
};

void Robot::stop() {};
