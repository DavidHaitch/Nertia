#include "propDefs/propDef.h"
#include "MotionState.h"
#include "vec3.h"
MotionState::MotionState()
{
    isEnabled = true;
}

long lastDebugPrintTime = 0;

Vec3 Transform(Vec3 value, float qx, float qy, float qz, float qw)
{
    Vec3 vector;
    float num12 = qx + qx;
    float num2 = qy + qy;
    float num = qz + qz;
    float num11 = qw * num12;
    float num10 = qw * num2;
    float num9 = qw * num;
    float num8 = qx * num12;
    float num7 = qx * num2;
    float num6 = qx * num;
    float num5 = qy * num2;
    float num4 = qy * num;
    float num3 = qz * num;
    float num15 = ((value.x * ((1.0 - num5) - num3)) + (value.y * (num7 - num9))) + (value.z * (num6 + num10));
    float num14 = ((value.x * (num7 + num9)) + (value.y * ((1.0 - num8) - num3))) + (value.z * (num4 - num11));
    float num13 = ((value.x * (num6 - num10)) + (value.y * (num4 + num11))) + (value.z * ((1.0 - num8) - num5));
    vector.x = num15;
    vector.y = num14;
    vector.z = num13;
    return vector;
}

int MotionState::Update(Adafruit_ICM20649 *imu)
{
    if (!isEnabled)
        return 0;

    long startTime = millis();
    lastUpdateTime = startTime;

    sensors_event_t a, g, temp;
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
    centripetalAccel = (angularVelocity * angularVelocity) * (PROPLENGTH / 2);

    float deltat = orientation.deltatUpdate();
    orientation.MadgwickUpdate(cgX, cgY, cgZ,
                               caX, caY, caZ,
                               deltat);

    // orientation.MahonyUpdate(scaled_cgX, scaled_cgY, scaled_cgZ,
    //                          scaled_caX, scaled_caY, scaled_caZ,
    //                          deltat);

    float accelMagnitude = sqrt((caX * caX) + (caY * caY) + (caZ * caZ));

    float accel = abs(sqrt((caX * caX) + (caY * caY) + (caZ * caZ)));

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
    float pitchRad = orientation.getPitchRadians();
    float rollRad = orientation.getRollRadians();
    // pointingX = cos(yawRad) * cos(pitchRad);
    // pointingY = sin(yawRad) * cos(pitchRad);
    // pointingZ = sin(pitchRad);

    qx = sin(rollRad / 2) * cos(pitchRad / 2) * cos(yawRad / 2) - cos(rollRad / 2) * sin(pitchRad / 2) * sin(yawRad / 2);
    qy = cos(rollRad / 2) * sin(pitchRad / 2) * cos(yawRad / 2) + sin(rollRad / 2) * cos(pitchRad / 2) * sin(yawRad / 2);
    qz = cos(rollRad / 2) * cos(pitchRad / 2) * sin(yawRad / 2) - sin(rollRad / 2) * sin(pitchRad / 2) * cos(yawRad / 2);
    qw = cos(rollRad / 2) * cos(pitchRad / 2) * cos(yawRad / 2) + sin(rollRad / 2) * sin(pitchRad / 2) * sin(yawRad / 2);
    Vec3 v(0, 1, 0);

    Vec3 vprime = Transform(v, qx, qy, qz, qw);
    pointingX = vprime.x;
    pointingY = vprime.y;
    pointingZ = vprime.z;
    // if (startTime - lastDebugPrintTime > 100)
    // {
    //     lastDebugPrintTime = startTime;

    //     // Serial.print(pointingX);
    //     // Serial.print("\t");
    //     // Serial.print(pointingY);
    //     // Serial.print("\t");
    //     // Serial.println(pointingZ);
    //     // Serial.print(orientation.getYawRadians());
    //     // Serial.print("\t");
    //     // Serial.print(orientation.getPitchRadians());
    //     // Serial.print("\t");
    //     // Serial.println(orientation.getRollRadians());
    //     String s = String("{\"time\":") + startTime
    //     + String(", \"type\":") + String("\"angles\"")
    //     + String(", \"qw\":") + qw
    //     + String(", \"qx\":") + qx
    //     + String(", \"qy\":") + qy
    //     + String(", \"qz\":") + qz
    //     + String(", \"x\":") + pointingX
    //     + String(", \"y\":") + pointingY
    //     + String(", \"z\":") + pointingZ
    //     + "}";
    //     Serial.println(s);
    // }

    long finishTime = millis();
    int dT = finishTime - startTime;
    return dT;
}