#ifndef PLASMAACTIVITY_H
#define PLASMAACTIVITY_H
#include "LedActivity.h"

// Derived from https://github.com/johncarl81/neopixelplasma/blob/master/neopixelplasma.ino

class PlasmaActivity : public LedActivity
{
public:
    PlasmaActivity(MotionState *_motionState, LedControl *_ledControl) : LedActivity(_motionState, _ledControl)
    {
    }

    bool enter(int param)
    {
        ledControl->minBrightness = 2;
        ledControl->addressingMode = Mirror;
    }

    bool update(bool realMode)
    {
        //phase = (abs(motionState->orientation.getYawRadians()) + abs(motionState->orientation.getRollRadians())) / 2;
        Point p1 = {1 + (sin(phase * 1.000)) * scale, 1 + (sin(phase * 1.310)) * scale, 1 + (sin(phase * 1.290)) * scale};
        Point p2 = {(sin(phase * 1.770)) * scale, (sin(phase * 2.865)) * scale, (sin(phase * 2.597)) * scale};
        for (int i = 0; i < TRUE_LEDS / 2; i++)
        {
            float r = baseDistance + (stepDistance * (float)i);
            Point p = {motionState->pointingX * r, motionState->pointingY * r, motionState->pointingZ * r};
           // Point p = {r,r,r};

            // Calculate the distance between this LED, and p1.
            Point dist1 = {p.x - p1.x, p.y - p1.y, p.z - p1.z}; // The vector from p1 to this LED.
            float distance1 = sqrt(dist1.x * dist1.x + dist1.y * dist1.y + dist1.z * dist1.z);

            // Calculate the distance between this LED, and p2.
            Point dist2 = {p.x - p2.x, p.y - p2.y, p.z - p2.z}; // The vector from p2 to this LED.
            float distance2 = sqrt(dist2.x * dist2.x + dist2.y * dist2.y + dist2.z * dist2.z);


            // Warp the distance with a sin() function. As the distance value increases, the LEDs will get light,dark,light,dark,etc...
            // You can use a cos() for slightly different shading, or experiment with other functions. Go crazy!
            float color_1 = distance1; // range: 0.0...1.0
            float color_2 = distance2;
            float color_4 = (sin(distance1 * distance2 * colorStretch)) + 2.0 * 0.5;

            // Square the color_f value to weight it towards 0. The image will be darker and have higher contrast.
            color_4 *= color_4;
            // color_1 *= color_1 * color_4;
            color_2 *= color_4;
            ledControl->leds[i] = CHSV(color_1 * 64.0, 255, color_2 * 64.0);
            //ledControl->leds[i].maximizeBrightness();
        }

        return true;
    }

    bool exit(int param)
    {
        return true;
    }

private:
    struct Point
    {
        float x;
        float y;
        float z;
    };
    float phase = 10.0;
    float scale = 2;
    float baseDistance = 1;
    float stepDistance = 1.0/4.0;
    const float colorStretch = 0.1; // Higher numbers will produce tighter color bands. I like 0.11 .
};
#endif