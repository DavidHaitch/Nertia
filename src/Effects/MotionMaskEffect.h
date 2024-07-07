#ifndef MOTIONMASKEFFECT_H
#define MOTIONMASKEFFECT_H
#include "../PatternMasks/sine8x28_thinTop.h"

#include "LedEffect.h"

class MotionMaskEffect : public LedEffect
{
public:
    MotionMaskEffect(MotionState *_motionState, LedControl *_ledControl) : LedEffect(_motionState, _ledControl)
    {
        mask = new sine8x28_thinTop_Mask();
        mask->setMirror(true);
    }

    bool apply(int param)
    {

        uint8_t currentStepDelay = map(motionState->angularVelocity * (180 / 3.14159), 0, 720, baseStepDelay, 0);
        if (currentStepDelay < 0)
            currentStepDelay = 0;
        if (micros() > nextStepChange)
        {
            mask->step();
            nextStepChange = micros() + currentStepDelay;
        }

        for (int i = 0; i < TRUE_LEDS / 2; i++)
        {
            ledControl->leds[i].fadeToBlackBy(mask->get(i));
        }

        return true;
    }

private:
    PatternMaskSource* mask;
    uint16_t baseStepDelay = 5000;
    long nextStepChange = 0;
    uint8_t stepCounter = 0;
};
#endif