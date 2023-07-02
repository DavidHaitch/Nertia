#ifndef COLORCLIMBACTIVITY_H
#define COLORCLIMBACTIVITY_H
#include "LedActivity.h"

#define CLIMB_DELAY 8000
class ColorclimbActivity : public LedActivity
{
public:
    ColorclimbActivity(MotionState *_motionState, LedControl *_ledControl, CRGBPalette16 *_palette) : LedActivity(_motionState, _ledControl)
    {
        palette = _palette;
    }

    bool enter(int param)
    {
        ledControl->minBrightness = 0;
        ledControl->addressingMode = Mirror;
        return true;
    }

    bool update(bool realMode)
    {
        if (micros() - lastShiftTime >= CLIMB_DELAY)
        {
            lastShiftTime = micros();
            coord += 1;
            float shiftVector = motionState->pointingZ;
#ifdef DART
            shiftVector *= -1;
#endif
            vShift += shiftVector;
        }

        int beat = beatsin8(60, 0, abs(motionState->pointingZ) * 255.0);
        for (int i = 0; i < TRUE_LEDS / 2; i++)
        {
            int r = baseDistance + (stepDistance * (i + vShift + 1));
            int color = 0;
            color = inoise16(beat << 8, r << 8) >> 8;
            color = qsub8(color, 32);
            color = qadd8(color, scale8(color, 39));
            ledControl->leds[i] = ColorFromPalette(*palette, color, inoise8(r, 10000), LINEARBLEND);
        }

        return true;
    }

    bool exit(int param)
    {
        return true;
    }

private:
    long coord;
    float vShift;
    long lastShiftTime;
    int baseDistance = 2;  // governs how drastically color changes with movement
    int stepDistance = 16; // governs how different each pixel is from the one before it.
    CRGBPalette16 *palette;
};
#endif