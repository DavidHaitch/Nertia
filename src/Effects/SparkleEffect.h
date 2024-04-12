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
        float axialError = 9.81 - abs(motionState->rawAxialAccel);
        if (motionState->jerk > 0.65 || (motionState->jerk > 0.2 && axialError > 3.0))
        {
            shaken = true;
        }

        EVERY_N_MILLIS(300)
        {
            if (!shaken && sparkles > 0)
                sparkles-=2;
        }
        EVERY_N_MILLIS(250)
        {
            if (shaken && sparkles < TRUE_LEDS - 4)
            {
                sparkles+=3;
            }

            shaken = false;
        }

        for (int i = 0; i < sparkles; i++)
        {
            ledControl->leds[sparkleIndices[i]] = CRGB::Red;
        }

        EVERY_N_MILLIS(sparkleTime)
        {
            for (int i = 0; i < sparkles; i++)
            {
                sparkleIndices[i] = rand() % (TRUE_LEDS);
            }
        }

        return false;
    }

private:
    bool shaken = false;
    int sparkleTime = 50;
    int sparkles = 0;
    int sparkleIndices[TRUE_LEDS];
};
#endif