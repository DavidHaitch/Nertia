#include "propDefs/propDef.h"

#include <SPI.h>
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

DEFINE_GRADIENT_PALETTE(pfoenix_p){
    0, 0, 0, 0,
    200, 96, 0, 255,
    255, 0, 0, 0};

CRGBPalette16 palette;

ColormapActivity colormap(&motionState, &ledControl, &palette, 16, 8);
ColormapActivity colormap_frantic(&motionState, &ledControl, &palette, 6000, 28);
ColorswingActivity colorswing(&motionState, &ledControl);
FiremapActivity firemap(&motionState, &ledControl);
GravityActivity gravity(&motionState, &ledControl);
FlashActivity flash(&motionState, &ledControl);
ColorsweepActivity colorsweep(&motionState, &ledControl, RainbowColors_p);
ColorsweepActivity pfoenix(&motionState, &ledControl, pfoenix_p);

PovActivity pov(&motionState, &ledControl);
SiezureActivity zap(&motionState, &ledControl);
PlasmaActivity plasma(&motionState, &ledControl);

#define NUM_BASE_ACTIVITIES 7
LedActivity *baseActivities[NUM_BASE_ACTIVITIES] =
    {
        &colormap,
        &firemap,
        &gravity,
        &colorsweep,
        &plasma,
        &zap,
        &colorswing};

LedEffect *effects[NUM_BASE_ACTIVITIES] =
    {
        &brightswing,
        &noop,
        &noop,
        &brightmap,
        &brightswing,
        &noop,
        &brightmap};
#define BRIGHTNESS_SETTINGS 3
int brightnesses[BRIGHTNESS_SETTINGS] = {8, 64, 200};

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
    int samples = 100;
    for (int i = 0; i < samples; i++)
    {
        vbat += getBatteryVolts();
    }

    vbat /= samples;

    int mapped = map(vbat * 10.0, 33, 42, 2, NUM_LEDS);
    Serial.println(vbat);
    ledControl.Clear();
    CRGB c = CRGB::Green;
    if (vbat >= 3.6 && vbat <= 4.0)
    {
        c = CRGB::Yellow;
    }

    if (vbat < 3.6)
    {
        c = CRGB::Red;
    }

    for (int i = 0; i < mapped; i++)
    {
        ledControl.leds[i] = c;
    }

    ledControl.addressingMode = Centered;

    ledControl.Refresh();
    delay(750);
}

void setup()
{
    Serial.begin(115200);
    //while (!Serial){}
    showBatteryVoltage();

    bool s = false;
    while (!imu.begin_SPI(ICM_CS))
    {
        Serial.println("Starting");
        digitalWrite(17, s);
        delay(250);
        s = !s;
    }

    imu.setAccelRange(ICM20649_ACCEL_RANGE_4_G);
    imu.setAccelRateDivisor(0);
    imu.setGyroRateDivisor(0);
    imu.setGyroRange(ICM20649_GYRO_RANGE_2000_DPS);
    ledControl.maxBrightness = brightnesses[0];

    base = baseActivities[0];
    base->enter(0);
    effect = effects[0];

    motionState.Update(&imu);
    srand((int)(motionState.orientation.getPitch() * 100.0));
    palette = RainbowColors_p;
    for (int i = 0; i < 8; i++)
    {
        int a = rand() % 16;
        int b = (a + 8) % 16;
        CRGB temp = palette[a];
        palette[a] = palette[b];
        palette[b] = temp;
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

    if (!configured && millis() - setupEndTime > 2000)
    {
        int c = config.configure(&motionState, &ledControl);

        if (c == 1)
        {
            ledControl.maxBrightness = brightnesses[config.options[0] % BRIGHTNESS_SETTINGS];
            base = transitionActivity(base, base); //Reinit base activity with new settings.
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

    if (effectEnable && configured)
    {
        effect->apply(0);
    }

    if (isIgniting)
    {
        isIgniting = ignite.apply(0);
    }

    int renderLag = millis() - renderStart;

    int pushStart = millis();
    ledControl.Refresh();
    int pushLag = millis() - pushStart;

    // Serial.print(motionLag);
    // Serial.print("\t");
    // Serial.print(renderLag);
    // Serial.print("\t");
    // Serial.print(pushLag);
    // Serial.print("\t");
    // Serial.println(millis() - start);
}