#include "Robot/Robot.hpp"

Robot::Robot(DataExchanger &dataExchanger, Accelerometer &accelerometer, Leds &leds, Lidar &lidar, MotorManager &motorManager) : dataExchanger(dataExchanger),
                                                                                                                                 accelerometer(accelerometer),
                                                                                                                                 leds(leds),
                                                                                                                                 lidar(lidar),
                                                                                                                                 motorManager(motorManager)

{
    start();
}

Robot::~Robot()
{
    stop();
}

void Robot::start() {

};

void Robot::stop() {

};

void Robot::moveMotorsForEncoderTicksCount(int64_t ticks, PowerMode powerMode)
{
    motorManager.moveMotorsForEncoderTicksCount(ticks, powerMode);
}

void Robot::rotateMotorsForEncoderTicksCount(int64_t ticks, PowerMode powerMode)
{
    motorManager.rotateMotorsForEncoderTicksCount(ticks, powerMode);
}
