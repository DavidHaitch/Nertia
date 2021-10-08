#ifndef MOTIONSTATE_H
#define MOTIONSTATE_H
#include "SensorFusion.h"

#include <Adafruit_ICM20649.h>
#include <Adafruit_Sensor.h>

class MotionState
{
public:
    MotionState();
    bool isEnabled;
    SF orientation;
    int relativeAngularVelocity;
    float angularVelocity;
    float angularAcceleration;
    int relativeAngularAcceleration;
    int relativeJerk;
    float jerk;
    float pointingX;
    float pointingY;
    float pointingZ;
    float qx;
    float qy;
    float qz;
    float qw;

    float rawAxialAccel;
    float centripetalAccel;
    int Update(Adafruit_ICM20649 *imu); // returns milliseconds taken
private:
    float maxAngularAcceleration;
    float maxAngularVelocity;
    float maxJerk;
    float lastAccel;
    float lastAngularVelocity;
    long lastUpdateTime;
};
#endif