#ifndef MOTIONMASKEFFECT_H
#define MOTIONMASKEFFECT_H

#include "LedEffect.h"
#define MOTIONMASK_ANIMATIONFRAMES 13
#define MOTIONMASK_ANIMATIONRESOLUTION 8

class MotionMaskEffect : public LedEffect
{
public:
    MotionMaskEffect(MotionState *_motionState, LedControl *_ledControl) : LedEffect(_motionState, _ledControl)
    {
    }

    bool apply(int param)
    {

        uint8_t currentStepDelay = map(motionState->angularVelocity * (180 / 3.14159), 0, 720, baseStepDelay, 0);
        if (currentStepDelay < 0)
            currentStepDelay = 0;
        if (micros() > nextStepChange)
        {
            stepCounter++;
            nextStepChange = micros() + currentStepDelay;
        }

        bool direction = true;
        for (int i = 0; i < TRUE_LEDS / 2; i++)
        {
            if (i % MOTIONMASK_ANIMATIONRESOLUTION == 0)
            {
                direction = !direction;
            }

            if (direction)
            {
                ledControl->leds[i].fadeToBlackBy(animationOffsets[stepCounter % MOTIONMASK_ANIMATIONFRAMES][i % MOTIONMASK_ANIMATIONRESOLUTION] * 31);
            }
            else
            {
                ledControl->leds[i].fadeToBlackBy(animationOffsets[stepCounter % MOTIONMASK_ANIMATIONFRAMES][MOTIONMASK_ANIMATIONRESOLUTION - (i % MOTIONMASK_ANIMATIONRESOLUTION)] * 31);
            }
        }

        return true;
    }

private:
    int animationOffsets[MOTIONMASK_ANIMATIONFRAMES][MOTIONMASK_ANIMATIONRESOLUTION] =
        {
            {8, 8, 0, 0, 0, 0, 0, 0},
            {8, 8, 0, 0, 0, 0, 0, 0},
            {0, 8, 8, 0, 0, 0, 0, 0},
            {0, 0, 8, 8, 0, 0, 0, 0},
            {0, 0, 0, 8, 8, 0, 0, 0},
            {0, 0, 0, 0, 8, 8, 8, 0},
            {0, 0, 0, 0, 0, 0, 8, 8},
            {0, 0, 0, 0, 0, 0, 8, 8},
            {0, 0, 0, 0, 0, 0, 8, 8},
            {0, 0, 0, 0, 0, 8, 8, 0},
            {0, 0, 0, 8, 8, 0, 0, 0},
            {0, 0, 8, 8, 0, 0, 0, 0},
            {0, 8, 8, 0, 0, 0, 0, 0},
            };
    uint16_t baseStepDelay = 5000;
    long nextStepChange = 0;
    uint8_t stepCounter = 0;
};
#endif