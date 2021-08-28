#ifndef COLORSWINGACTIVITY_H
#define COLORSWINGACTIVITY_H
#include "LedActivity.h"

class ColorswingActivity : public LedActivity {
public:
    ColorswingActivity(MotionState* _motionState, LedControl* _ledControl) : LedActivity(_motionState, _ledControl)
    {
        //palette = CRGBPalette16(CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow);
        palette = OceanColors_p;
    }

    bool enter(int param)
    {
        ledControl->minBrightness = 0;
        ledControl->addressingMode = Mirror;
    }

    bool update(bool realMode)
    {
        int c = motionState->relativeAngularVelocity;
        CRGB color = ColorFromPalette( palette, c, 255, NOBLEND);
        for (int i = 0; i < TRUE_LEDS / 2; i++)
        {
            ledControl->leds[i] = color;
        }
         
        return true;
    }

    bool exit(int param)
    {
    }
private:
    CRGBPalette16 palette;
};
#endif