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
        EVERY_N_MILLIS(100)
        {
            if (sparkles > 0)
                sparkles--;
        }
        EVERY_N_MILLIS(200)
        {
            if (sparkles < TRUE_LEDS / 4)
                sparkles += map(angVel, 0,2160, 0, 6);
        }

        for (int i = 0; i < sparkles; i++)
        {
            ledControl->leds[sparkleIndices[i]] = CRGB::White;
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
    int sparkleTime = 50;
    int sparkles = 0;
    int sparkleIndices[TRUE_LEDS];
};
#endif