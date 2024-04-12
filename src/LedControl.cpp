#include "LedControl.h"
const int refreshRate = 0;
LedControl::LedControl()
{
    brightness = 255;
    minBrightness = 0;
    maxBrightness = 255;
    addressingMode = Direct;
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(trueLeds, TRUE_LEDS).setCorrection(TypicalLEDStrip);
    Clear();
}

void LedControl::Refresh()
{
    long now = micros();
    if (now - lastUpdate < fps)
    {
        return;
    }

    lastUpdate = micros();
    uint8_t b = map(brightness, 0, 255, minBrightness, maxBrightness);
    FastLED.setBrightness(b);

    if (addressingMode == Centered)
    {
        //Segment One
        for (int i = 0; i < NUM_LEDS; i++)
        {
            trueLeds[i] = leds[NUM_LEDS - i];
        }

        //Segment Two

        // for (int i = 0; i < NUM_LEDS; i++)
        // {
        //     trueLeds[(NUM_LEDS) + i] = trueLeds[(NUM_LEDS) - i - 1];
        // }

        //Mirror side 1 onto side 2
        for (int i = 0; i < TRUE_LEDS / 2; i++)
        {
            trueLeds[(TRUE_LEDS / 2) + i] = trueLeds[(TRUE_LEDS / 2) - i - 1];
        }
    }
    else if (addressingMode == Mirror)
    {
        for (int i = 0; i < TRUE_LEDS / 2; i++)
        {
            trueLeds[i] = leds[i];
            trueLeds[(TRUE_LEDS / 2) + i] = leds[(TRUE_LEDS / 2) - i - 1];
        }
    }
    else if (addressingMode == Direct)
    {
        for (int i = 0; i < TRUE_LEDS; i++)
        {
            trueLeds[i] = leds[i];
        }
    }

    FastLED.show();
}

void LedControl::Clear()
{
    for (int i = 0; i < TRUE_LEDS; i++)
    {
        trueLeds[i] = CRGB::Black;
    }

    FastLED.show();
}