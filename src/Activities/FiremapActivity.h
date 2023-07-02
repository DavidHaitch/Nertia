#ifndef FIREMAPACTIVITY_H
#define FIREMAPACTIVITY_H
#include "LedActivity.h"

class FiremapActivity : public LedActivity
{
public:
    FiremapActivity(MotionState *_motionState, LedControl *_ledControl) : LedActivity(_motionState, _ledControl)
    {
        palette = CRGBPalette16(
            0x000000, 0x000033, 0x001166, 0x001199,
            0x0022CC, 0x0033FF, 0x0044FF, 0x0066FF,
            0x0099FF, 0x00CCFF, 0x00FFFF, 0x33FFFF,
            0x33FFFF, 0x3366FF, 0xCCFFFF, 0xFFFFFF);
        palette = HeatColors_p;
    }

    bool enter(int param)
    {
        ledControl->minBrightness = 4;
        ledControl->addressingMode = Mirror;
        return true;
    }

    bool update(bool realMode)
    {
        long now = millis();
        if (now - lastFireTick > 8)
        {
            lastFireTick = now;
            int angVel = (motionState->angularVelocity * (180 / 3.14159));
#ifdef DART
            int spinout = 360 * 12;

#endif
#ifdef BATON
            int spinout = 360 * 6;
#endif
#ifdef STAFF
            int spinout = 360 * 4;
#endif
            for (int i = 0; i < TRUE_LEDS / 2; i++)
            {
                int coolingFactor = map(angVel, 0, spinout, 2, 8);
                int temperature = map(angVel, 0, spinout, 170, 240);
                int heatChance = map(angVel, 0, spinout, 4, 24);
                if (angVel > spinout)
                {
                    temperature /= 2;
                    heatChance /= 8;
                    // coolingFactor *= 2;
                }

                if (heat[i] > coolingFactor)
                {
                    heat[i] -= coolingFactor;
                }
                else if (angVel > spinout && rand() % 255 < 8)
                {
                    heat[i] = (-1 * rand() % 1024) - 512;
                }
                else if (heat[i] > 0)
                {
                    heat[i] = 0;
                }

                if (rand() % 255 < heatChance && heat[i] < temperature)
                {
                    heat[i] += temperature;
                }
            }
        }

        if (now - lastFireRise > 2)
        {
            lastFireRise = now;
            // cA = angVel^2 * radius
            float accel = motionState->rawAxialAccel * -1;

            if (motionState->centripetalAccel > abs(accel))
            {
                for (int i = TRUE_LEDS / 4; i > 0; i--)
                {
                    if (heat[i] >= 0 && heat[i - 1] >= 0)
                    {
                        heat[i] += heat[i - 1] * 0.75;
                        heat[i - 1] -= heat[i - 1] * 0.75;
                    }
                }
                for (int i = 0; i < (TRUE_LEDS / 4); i++)
                {
                    if (heat[i] >= 0 && heat[i + 1] >= 0)
                    {
                        heat[i] += heat[i + 1] * 0.75;
                        heat[i + 1] -= heat[i + 1] * 0.75;
                    }
                }
            }
            else
            {
#ifdef DART
                if (motionState->rawAxialAccel > 0)
#endif
#ifndef DART
                    if (motionState->rawAxialAccel < 0)
#endif
                    {
                        for (int i = TRUE_LEDS / 2; i > 0; i--)
                        {
                            if (heat[i] >= 0 && heat[i - 1] >= 0)
                            {
                                heat[i] += heat[i - 1] * 0.75;
                                heat[i - 1] -= heat[i - 1] * 0.75;
                            }
                        }
                    }
                    else
                    {
                        for (int i = 0; i < (TRUE_LEDS / 2) - 1; i++)
                        {
                            if (heat[i] >= 0 && heat[i + 1] >= 0)
                            {
                                heat[i] += heat[i + 1] * 0.75;
                                heat[i + 1] -= heat[i + 1] * 0.75;
                            }
                        }

                        heat[0] = 0;
                    }
            }
        }

        for (int i = 0; i < TRUE_LEDS / 2; i++)
        {
            if (heat[i] >= 0)
            {
                ledControl->leds[i] = ColorFromPalette(palette, heat[i], 255, LINEARBLEND);
            }
            else
            {
                int b = (heat[i] * -1) / 64;
                ledControl->leds[i] = CRGB(b / 2, b / 2, b);
            }
        }

        // blur1d(ledControl->leds, NUM_LEDS / 2, 64);
        // blur1d(ledControl->leds, NUM_LEDS / 2, 92);
        return true;
    }

    bool exit(int param)
    {
        return true;
    }

private:
    int heat[TRUE_LEDS];
    long lastFireTick = 0;
    long lastFireRise = 0;
    CRGBPalette16 palette;
};
#endif