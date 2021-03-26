#include "propDefs/propDef.h"
#include "MotionState.h"
MotionState::MotionState()
{
    isEnabled = true;
}

int MotionState::Update(Adafruit_ICM20649 *imu)
{
    if (!isEnabled)
        return 0;

    long startTime = millis();
    lastUpdateTime = startTime;
    
    sensors_event_t a,  g, temp;
    imu->getEvent(&a, &g, &temp);
    float cgX = g.gyro.x;
    float cgY = g.gyro.y;
    float cgZ = g.gyro.z;

    // Y axis is not factored into angular velocity, because that represents the roll axis.
    // For applications where the SmartFlow board does not point along the axis of the prop, this must be changed.
    angularVelocity = abs(cgX) + abs(cgZ);

    float caX = a.acceleration.x;
    float caY = a.acceleration.y;
    float caZ = a.acceleration.z;

    rawAxialAccel = caY;

    // cA = angVel^2 * radius 
    centripetalAccel = (angularVelocity * angularVelocity) * (PROPLENGTH/2);


    float deltat = orientation.deltatUpdate();
    orientation.MadgwickUpdate(cgX, cgY, cgZ,
                               caX, caY, caZ,
                               deltat);

    // orientation.MahonyUpdate(scaled_cgX, scaled_cgY, scaled_cgZ,
    //                          scaled_caX, scaled_caY, scaled_caZ,
    //                          deltat);

    float accel = abs(caX) + abs(caY) + abs(caZ);

    jerk = abs(accel - lastAccel);
    if (jerk > maxJerk)
    {
        maxJerk = jerk;
        relativeJerk = 255;
    }

    if (maxJerk < 1.0 * jerk)
    {
        //We don't have enough samples to really make sense.
        relativeJerk = 0;
    }
    else
    {
        relativeJerk = (jerk / maxJerk) * 255;
    }

    lastAccel = accel;

    if (angularVelocity > maxAngularVelocity)
    {
        maxAngularVelocity = angularVelocity;
        relativeAngularVelocity = 255;
    }
    else
    {
        relativeAngularVelocity = (angularVelocity / maxAngularVelocity) * 255.0;
    }

    maxAngularVelocity *= 0.999;

    angularAcceleration = abs(angularVelocity - lastAngularVelocity);
    lastAngularVelocity = angularVelocity;
    if (angularAcceleration > maxAngularAcceleration)
    {
        maxAngularAcceleration = angularAcceleration;
        relativeAngularAcceleration = 255;
    }

    if (maxAngularAcceleration < 1 * angularAcceleration)
    {
        //We don't have enough samples to really make sense.
        relativeAngularAcceleration = 0;
    }
    else
    {
        relativeAngularAcceleration = (angularAcceleration / maxAngularAcceleration) * 255;
    }

    float yawRad = orientation.getYawRadians();
    float pitchRad = orientation.getRollRadians();

    pointingX = cos(yawRad) * cos(pitchRad);
    pointingY = sin(yawRad) * cos(pitchRad);
    pointingZ = sin(pitchRad);

    // Serial.print(pointingX);
    // Serial.print("\t");
    // Serial.print(pointingY);
    // Serial.print("\t");
    // Serial.println(pointingZ);

    long finishTime = millis();
    int dT = finishTime - startTime;
    return dT;
}