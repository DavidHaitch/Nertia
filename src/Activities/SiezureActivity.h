#ifndef SIEZUREACTIVITY_H
#define SIEZUREACTIVITY_H
#include "LedActivity.h"

class SiezureActivity : public LedActivity {
public:
    SiezureActivity(MotionState* _motionState, LedControl* _ledControl) : LedActivity(_motionState, _ledControl)
    {
        colors[0] = CRGB::Red;
        colors[1] = CRGB::Green;
        colors[2] = CRGB::Blue;
    }

    bool enter(int param)
    {
        ledControl->minBrightness = 4;
        ledControl->addressingMode = Centered;
    }

    bool update(bool realMode)
    {
        if(realMode)
        {
            motionState->isEnabled = false;
        }

        if(count%4 == 0)
        {
            for (int i = 0; i < NUM_LEDS; i++)
            {
                ledControl->leds[i] = CRGB::Black;
            }
        }
        else
        {
            for (int i = 0; i < NUM_LEDS; i++)
            {
                ledControl->leds[i] = CHSV(((count+i)*17)%255, 255,255);//colors[count%3];
            }

        }

        if(millis() - lastShift >= shiftDelay)
        {
            offset++;
            count++;
            lastShift = millis();
        }

        return true;
    }

    bool exit(int param)
    {
    }
private:
    CRGB colors[3];
    int offset;
    int count;
    const int shiftDelay = 5;
    long lastShift;
};
#endif