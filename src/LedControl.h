#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include <FastLED.h>
#include "propDefs/propDef.h"
#define COLOR_ORDER BGR
#define DATA_PIN    17
#define CLOCK_PIN   18
#define LED_TYPE    SK9822

enum LedAddressingMode {Direct, Mirror, Centered};

class LedControl
{
public:
    LedControl();
    uint8_t brightness;
    uint8_t maxBrightness;
    uint8_t minBrightness;
    CRGB leds[TRUE_LEDS];
    CRGB trueLeds[TRUE_LEDS];
    void Clear();
    void Refresh();
    LedAddressingMode addressingMode;
private:
    long lastUpdate;
    const int fps = 100000 / 240;
};
#endif