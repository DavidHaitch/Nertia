#ifndef BRIGHTMAPEFFECT_H
#define BRIGHTMAPEFFECT_H
#include "LedEffect.h"

class BrightmapEffect : public LedEffect
{
public:
  BrightmapEffect(MotionState *_motionState, LedControl *_ledControl) : LedEffect(_motionState, _ledControl)
  {
  }

  bool apply(int param)
  {
    for (int i = 0; i < TRUE_LEDS / 2; i++)
    {
      float r = baseDistance + (stepDistance * (i + 1.0));
      int b = inoise8(abs(motionState->pointingX) * r, abs(motionState->pointingY) * r, abs(motionState->pointingZ) * r);
      b = qsub8(b, 16);
      b = qadd8(b, scale8(b, 39));
      if (b < 96)
        b = 0;
      if (b > 192)
        b = 255;
      ledControl->leds[i].fadeLightBy(b);
    }

    return true;
  }

private:
  int baseDistance = 800;  // governs how drastically color changes with movement
  float stepDistance = 10; //governs how different each pixel is from the one before it.
};
#endif