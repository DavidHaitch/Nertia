#ifndef COLORSWEEPACTIVITY_H
#define COLORSWEEPACTIVITY_H
#include "LedActivity.h"

#define SWEEP_DELAY 80
class ColorsweepActivity : public LedActivity
{
public:
    ColorsweepActivity(MotionState *_motionState, LedControl *_ledControl, CRGBPalette16 _palette) : LedActivity(_motionState, _ledControl)
    {
        palette = _palette;
    }

    bool enter(int param)
    {
        ledControl->minBrightness = 0;
        ledControl->addressingMode = Mirror;
    }

    bool update(bool realMode)
    {
        if (realMode)
        {
            // motionState->isEnabled = false;
        }

        if (micros() - lastShiftTime >= SWEEP_DELAY)
        {
            lastShiftTime = micros();
            coord += coordRateOfChange;
        }

        EVERY_N_MILLIS(250)
        {
            int angVel = (motionState->angularVelocity * (180 / 3.14159));
            int targetRateOfChange = map(angVel, 0, 360, 1, 8);
            if(coordRateOfChange > targetRateOfChange) coordRateOfChange--;
            if(coordRateOfChange < targetRateOfChange) coordRateOfChange++;
        }

        // fill_noise8(ledControl->leds, TRUE_LEDS/2,
        // 1, 0, 16,
        // 2, 0, 16,coord);

        for (int i = 0; i < TRUE_LEDS / 2; i++)
        {
            int r = baseDistance + (stepDistance * (i + 1));
            int color = 0;
            color = inoise16(coord << 8, r << 8) >> 8;
            color = qsub8(color, 32);
            color = qadd8(color, scale8(color, 39));
            ledControl->leds[i] = ColorFromPalette(palette, color + offset, 255, LINEARBLEND);
        }

        return true;
    }

    bool exit(int param)
    {
    }

private:
    long coord;
    int coordRateOfChange;
    int offset;
    long lastShiftTime;
    int baseDistance = 2;  // governs how drastically color changes with movement
    int stepDistance = 32; // governs how different each pixel is from the one before it.
    CRGBPalette16 palette;
};
#endif