#ifndef IGNITEEFFECT_H
#define IGNITEEFFECT_H
#include "LedEffect.h"

class IgniteEffect : public LedEffect {
public:
    IgniteEffect(MotionState* _motionState, LedControl* _ledControl) : LedEffect(_motionState, _ledControl)
    {
    }

    bool apply(int param)
    {
        if (ignitedLeds <= TRUE_LEDS / 2)
        {
            if(millis() - lastChangeTime >= 4)
            {
                ignitedLeds++;
                lastChangeTime = millis();
            }

            for (int i = 0; i < TRUE_LEDS; i++)
            {
                if (i < ignitedLeds || TRUE_LEDS - i < ignitedLeds)
                {
                    continue;
                }
                else
                {
                    ledControl->leds[i] = CRGB::Black;
                }
            }

            return true;
        }       
        else
        {
            return false;
        }
    }
private:
    long lastChangeTime;
    uint8_t ignitedLeds;
};
#endif