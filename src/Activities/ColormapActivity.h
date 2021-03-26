#ifndef COLORMAPACTIVITY_H
#define COLORMAPACTIVITY_H
#include "LedActivity.h"
#include "../propDefs/propDef.h"

class ColormapActivity : public LedActivity {
public:
    ColormapActivity(MotionState* _motionState, LedControl* _ledControl, CRGBPalette16* _palette, int _baseDistance, int _stepDistance) : LedActivity(_motionState, _ledControl)
    {
        palette = _palette;
        baseDistance = _baseDistance;
        stepDistance = _stepDistance;
    }

    bool enter(int param)
    {
        ledControl->minBrightness = 8;
        #ifndef STAFF
        ledControl->addressingMode = Mirror;
        #else
        ledControl->addressingMode = Centered;
        #endif
    }

    bool update(bool realMode)
    {
        if(millis() - lastShiftDecay > 200)
        {
            lastShiftDecay = millis();
            if(shift > 0) shift--;
        }

        shift = motionState->orientation.getPitch();

        for (int i = 0; i < TRUE_LEDS / 2; i++)
        {
            float r = baseDistance + (stepDistance * i);
            int c = inoise8(motionState->pointingX * r, motionState->pointingY * r, motionState->pointingZ * r);
            CRGB color = ColorFromPalette(*palette, c + shift, 255, LINEARBLEND);
            ledControl->leds[i] = color;
        }
                
        return true;
    }

    bool exit(int param)
    {
    }

private:
    long lastShiftDecay = 0;
    uint8_t shift = 0;
    int baseDistance; // governs how drastically color changes with movement
    int stepDistance; //governs how different each pixel is from the one before it.
    CRGBPalette16* palette;
    CRGBPalette16 warmPalette;
    CRGBPalette16 coolPalette;
    int secondSampleOffset = 10000;
};
#endif