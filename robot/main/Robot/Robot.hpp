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
    Robot(DataExchanger &dataExchanger, Accelerometer &accelerometer, Leds &leds, Lidar &lidar, MotorManager &motorManager);
    ~Robot();

    DataExchanger &dataExchanger;
    void moveMotorsForEncoderTicksCount(int ticks, PowerMode powerMode);
    void rotateMotorsForEncoderTicksCount(int ticks, PowerMode powerMode);

private:
    Accelerometer &accelerometer;
    Leds &leds;
    Lidar &lidar;
    MotorManager &motorManager;

    State state = stopped;

    void start();
    void stop();
};
