#ifndef BRIGHTSWINGEFFECT_H
#define BRIGHTSWINGEFFECT_H
#include "LedEffect.h"

class BrightswingEffect : public LedEffect
{
public:
    BrightswingEffect(MotionState *_motionState, LedControl *_ledControl) : LedEffect(_motionState, _ledControl)
    {
    }

    bool apply(int param)
    {
        ledControl->brightness = motionState->relativeAngularVelocity;

        return false;
    }
};
#endif