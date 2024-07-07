//#define SERIALDEBUG
#include "arduino_m0_tweak.hpp"
#include "propDefs/propDef.h"

#include <Adafruit_ICM20649.h>
#define ICM_CS 3
#include <Adafruit_Sensor.h>
#include "MotionState.h"
#include "LedControl.h"
#include "Activities.h"
#include "Effects.h"

Adafruit_ICM20649 imu = Adafruit_ICM20649();

MotionState motionState;
LedControl ledControl;

IgniteEffect ignite(&motionState, &ledControl);
BrightswingEffect brightswing(&motionState, &ledControl);
FireswingEffect fireswing(&motionState, &ledControl);
BrightmapEffect brightmap(&motionState, &ledControl);
SparkleEffect sparkle(&motionState, &ledControl);
NoopEffect noop(&motionState, &ledControl);
MarchingEffect marching(&motionState, &ledControl);
MotionMaskEffect motionMask(&motionState, &ledControl);
DEFINE_GRADIENT_PALETTE(pfoenix_p){
    0, 0, 0, 0,
    200, 96, 0, 255,
    255, 0, 0, 0};

CRGBPalette16 palette;
CRGBPalette16 oceanPalette = OceanColors_p;

ColormapActivity colormap(&motionState, &ledControl, &palette, 10, 10);
ColormapActivity colormap_frantic(&motionState, &ledControl, &palette, 6000, 28);
ColorswingActivity colorswing(&motionState, &ledControl);
FiremapActivity firemap(&motionState, &ledControl);
GravityActivity gravity(&motionState, &ledControl);
FlashActivity flash(&motionState, &ledControl);
ColorsweepActivity colorsweep(&motionState, &ledControl, &palette);
ColorclimbActivity colorclimb(&motionState, &ledControl, &palette);
ColorsweepActivity colorsweep_waterbend(&motionState, &ledControl, &oceanPalette);

PovActivity pov(&motionState, &ledControl);
SiezureActivity zap(&motionState, &ledControl);
PlasmaActivity plasma(&motionState, &ledControl);

#define NUM_BASE_ACTIVITIES 8
LedActivity *baseActivities[NUM_BASE_ACTIVITIES] =
    {
        &colormap,
        &firemap,
        &colorclimb,
        &colorsweep,
        &gravity,
        &plasma,
        &zap,
        &colorswing};

LedEffect *effects[NUM_BASE_ACTIVITIES] =
    {
        &noop,
        &noop,
        &sparkle,
        &noop,
        &brightmap,
        &brightswing,
        &noop,
        &brightmap};

#ifndef LIZARDTAIL
#define BRIGHTNESS_SETTINGS 3
int brightnesses[BRIGHTNESS_SETTINGS] = {16, 64, 128};
#else
#define BRIGHTNESS_SETTINGS 3 // Lizard tail fails at high brightnesses
int brightnesses[BRIGHTNESS_SETTINGS] = {16, 32, 64};
#endif

LedActivity *base;
LedEffect *effect;
ConfigManager config;

long lastDebugPrint = 0;
bool effectEnable = false;
bool isIgniting = true;

long setupEndTime = 0;
float getBatteryVolts()
{
    float measuredvbat = analogRead(A7);
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage
    return measuredvbat;
}

void showBatteryVoltage()
{
    float vbat = 0.0;

    int samples = 20;
    for (int i = 0; i < samples; i++)
    {
        vbat += getBatteryVolts();
        delay(1);
    }

    vbat /= samples;

    int mapped = map(vbat * 10.0, 33, 42, 2, NUM_LEDS);
    if (mapped < 3)
    {
        mapped = 3;
    }

    ledControl.Clear();
    CRGB c = CRGB::Green;
    if (vbat >= 3.6 && vbat <= 3.9)
    {
        c = CRGB::Yellow;
    }

    if (vbat < 3.6)
    {
        c = CRGB::Red;
    }

    ledControl.addressingMode = Centered;
    long displayStart = millis();
    while (millis() - displayStart < 750)
    {
        CRGB indicatorColor = c;
        if (vbat < 3.4 && (millis() / 50) % 2 == 0)
        {
            indicatorColor = CRGB::Black;
        }

        for (int i = 0; i < mapped; i++)
        {
            ledControl.leds[i] = indicatorColor;
        }
        ledControl.Refresh();
    }
}

void setup()
{
     m0tweak::cpuFrequency(48);
#ifdef SERIALDEBUG
    Serial.begin(115200);
    while (!Serial)
    {
    }
#endif

#ifndef LIZARDTAIL // The lizard tail's vbat readout is shot
    showBatteryVoltage();
#endif

    bool s = false;
    while (!imu.begin_SPI(ICM_CS))
    {
#ifdef SERIALDEBUG
        Serial.println("Starting IMU");
#endif
        digitalWrite(17, s);
        delay(50);
        s = !s;
    }

    imu.setAccelRange(ICM20649_ACCEL_RANGE_4_G);
    imu.setAccelRateDivisor(0);
    imu.setGyroRateDivisor(0);
    imu.setGyroRange(ICM20649_GYRO_RANGE_1000_DPS);
    ledControl.maxBrightness = brightnesses[0];

    base = baseActivities[0];
    base->enter(0);
    effect = effects[0];

    motionState.Update(&imu);
    srand((int)(motionState.orientation.getPitch() * 100.0));
    palette = RainbowColors_p;
    for (int i = 0; i < 8; i++)
    {
        // int a = rand() % 16;
        // int b = (a + 8) % 16;
        // CRGB temp = palette[a];
        // palette[a] = palette[b];
        // palette[b] = temp;
    }
    setupEndTime = millis();
}

LedActivity *transitionActivity(LedActivity *from, LedActivity *to)
{
    from->exit(0);
    to->enter(0);
    return to;
}

bool configured = false;
void loop()
{
    long start = millis();
    int motionLag = motionState.Update(&imu);
    long renderStart = millis();
    base->update(configured);

    if (!configured && millis() - setupEndTime > 1000)
    {
        int c = config.configure(&motionState, &ledControl);

        if (c == 1)
        {
            ledControl.maxBrightness = brightnesses[config.options[0] % BRIGHTNESS_SETTINGS];
            base = transitionActivity(base, base); // Reinit base activity with new settings.
        }

        if (c == 2)
        {
            uint8_t mode = config.options[1] % NUM_BASE_ACTIVITIES;
            ledControl.Clear();
            base = transitionActivity(base, baseActivities[mode]);
            effect = effects[mode];
        }

        if (c >= 3)
        {
            effectEnable = true;
            configured = true;
        }
    }

        effect->apply(0);

    if (effectEnable && configured)
    {
        //effect->apply(0);
    }

    if (isIgniting)
    {
        isIgniting = ignite.apply(0);
    }

    int renderLag = millis() - renderStart;
    int pushStart = millis();
    ledControl.Refresh();
    int pushLag = millis() - pushStart;
#ifdef SERIALDEBUG
    if (start - lastDebugPrint > 16)
    {
        lastDebugPrint = start;
        String s = String("{\n\t\"time\":") + start + ",\n";
        s += String("\t\"type\":") + String("\"angles\"") + ",\n";
        s += String("\t\"motionLag\":") + motionLag + ",\n";
        s += String("\t\"renderLag\":") + renderLag + ",\n";
        s += String("\t\"pushLag\":") + pushLag + ",\n";
        s += String("\t\"totalLag\":") + (millis() - start) + ",\n";
        s += String("\t\"qw\":") + motionState.qw + ",";
        s += String("\"qx\":") + motionState.qx + ",";
        s += String("\"qy\":") + motionState.qy + ",";
        s += String("\"qz\":") + motionState.qz + ",\n";
        s += String("\t\"x\":") + motionState.pointingX + ",\n";
        s += String("\t\"y\":") + motionState.pointingY + ",\n";
        s += String("\t\"z\":") + motionState.pointingZ + "\n";
        s += "}";
        Serial.println(s);
    }
#endif
}