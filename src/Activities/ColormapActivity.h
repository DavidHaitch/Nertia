#ifndef COLORMAPACTIVITY_H
#define COLORMAPACTIVITY_H
#include "LedActivity.h"
#include "../propDefs/propDef.h"

class ColormapActivity : public LedActivity
{
public:
    ColormapActivity(MotionState *_motionState, LedControl *_ledControl, CRGBPalette16 *_palette, int _baseDistance, int _stepDistance) : LedActivity(_motionState, _ledControl)
    {
        palette = _palette;
        baseDistance = _baseDistance;
        stepDistance = _stepDistance;
    }

    bool enter(int param)
    {
        ledControl->minBrightness = 8;
#ifdef DART
        ledControl->addressingMode = Mirror;
#else
        ledControl->addressingMode = Centered;
#endif
        return true;
    }

    bool update(bool realMode)
    {
        if (millis() - lastShiftStep > 800)
        {
            lastShiftStep = millis();
            shift++;
        }

#ifndef DART
        uint8_t spatialShift = abs(motionState->pointingZ) * 1;
#else
        uint8_t spatialShift = motionState->pointingZ * 1;
#endif

        for (int i = 0; i < TRUE_LEDS / 2; i++)
        {
            float r = baseDistance + (stepDistance * i);
            int c = inoise8(motionState->pointingX * r, motionState->pointingY * r, motionState->pointingZ * r) * 2;
            CRGB color = ColorFromPalette(*palette, c + shift + spatialShift, 255, LINEARBLEND);
            ledControl->leds[i] = color;
        }

        return true;
    }

    bool exit(int param)
    {
        return true;
    }

private:
    long lastShiftStep = 0;
    uint8_t shift = 0;
    int baseDistance; // governs how drastically color changes with movement
    int stepDistance; // governs how different each pixel is from the one before it.
    CRGBPalette16 *palette;
    CRGBPalette16 warmPalette;
    CRGBPalette16 coolPalette;
    int secondSampleOffset = 10000;
};
#endif