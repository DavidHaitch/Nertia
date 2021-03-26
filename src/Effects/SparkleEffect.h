#ifndef SPARKLEEFFECT_H
#define SPARKLEEFFECT_H
#include "LedEffect.h"

class SparkleEffect : public LedEffect
{
public:
    SparkleEffect(MotionState *_motionState, LedControl *_ledControl) : LedEffect(_motionState, _ledControl)
    {
    }

    bool apply(int param)
    {

        int angVel = (motionState->angularVelocity * (180 / 3.14159));
        if (abs(angVel) < 90)
        {
            if (motionState->rawAxialAccel > 0)
            {
                if (lastInversion < 0)
                    lastInversion = millis();
                if (millis() - lastInversion > 5000)
                {
                    EVERY_N_SECONDS(1)
                    {
                        if (sparkles < NUM_LEDS / 4)
                            sparkles++;
                    }
                }
            }
            else
            {
                lastInversion = -1;
                EVERY_N_MILLIS(100)
                {
                    if (sparkles > 0)
                        sparkles--;
                }
            }
        }

        for (int i = 0; i < sparkles; i++)
        {
            ledControl->leds[sparkleIndices[i]] = CRGB::White;
        }

        EVERY_N_MILLIS(sparkleTime)
        {
            for (int i = 0; i < sparkles; i++)
            {
                sparkleIndices[i] = rand() % (NUM_LEDS);
            }
        }

        return false;
    }

private:
    int sparkleTime = 35;
    int sparkles = 0;
    int sparkleIndices[NUM_LEDS];
    long lastInversion = 0;
};
#endif