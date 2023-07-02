#ifndef GRAVITYACTIVITY_H
#define GRAVITYACTIVITY_H
#include "LedActivity.h"

class GravityActivity : public LedActivity
{
public:
    GravityActivity(MotionState *_motionState, LedControl *_ledControl) : LedActivity(_motionState, _ledControl)
    {
    }

    bool enter(int param)
    {
        ledControl->minBrightness = 0;
        ledControl->addressingMode = Mirror;
        return true;
    }

    bool update(bool realMode)
    {
        if (millis() - lastCalc > wait)
        {
            lastCalc = millis();
            // float incline = (motionState->orientation.getPitch());
            // float accel = gravity * sin(incline * (3.14159/180));
            float accel = motionState->rawAxialAccel * -1;

            if (motionState->centripetalAccel > abs(accel))
            {
                accel = motionState->centripetalAccel;
                if (dot < (TRUE_LEDS / 2) * 50)
                {
                    accel *= -1;
                }
            }

            accel *= 3.0;

            speed += accel;
            dot += speed;
            if (dot < 0)
            {
                dot = 10;
                hue += abs(speed) / 32;
                speed = 0;
            }

            if (dot > (TRUE_LEDS / 2) * 100)
            {
                dot = ((TRUE_LEDS / 2) * 100) - 10;
                hue += abs(speed) / 32;
                speed = 0;
            }

            // if(abs(incline) < stickiness) speed = 0;
        }

        float antiDot = ((TRUE_LEDS / 2) * 100) - dot;
        for (int i = 0; i < TRUE_LEDS / 2; i++)
        {
            float dist = abs((i * 100) - dot);
            float antiDist = abs((i * 100) - antiDot);
            if (dist < 512)
            {
                ledControl->leds[i] = ColorFromPalette(CloudColors_p, dist / 2, 255, LINEARBLEND);
                ledControl->leds[i].fadeToBlackBy(dist / 8);
            }

            if (antiDist < 512)
            {
                ledControl->leds[i] = ColorFromPalette(ForestColors_p, dist / 2, 255, LINEARBLEND);
                ledControl->leds[i].fadeToBlackBy(antiDist / 8);
            }

            if (dist > 512 && antiDist > 512)
            {
                ledControl->leds[i] = CRGB::Black;
            }
        }

        return true;
    }

    bool exit(int param)
    {
        return true;
    }

private:
    int hue = 0;
    float dot = 0;
    float stickiness = 2.5;
    float gravity = 30.0;
    float speed = 0;
    uint16_t wait = 1000 / 60;
    long lastCalc = 0;
};
#endif