#include <Arduino.h>
#include <FastLED.h>
#include "../../../contracts/basetypes.h"

#define LED_PIN 3

// Information about the LED strip itself

#define NUM_LEDS 60

#define CHIPSET WS2811

#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

//current Animation variables:
uint32_t currentFrame = 0;
uint16_t currentSpeedVal = 0;

// Animations Variablen, die über MQTT aktuallisiert werden:
struct ledState myLedState = {(uint8_t)128, (uint16_t)1500, (uint8_t)0, (uint16_t)0, (uint8_t)2, (uint8_t)0, (uint32_t)0xAA0000};

void fillColor(CRGB color, uint8_t from = 0, uint8_t to = NUM_LEDS)
{
    for (int i = from; i < to; i++)
    {
        leds[i] = color;
    }
}

void off()
{
    fillColor(CRGB::Black);
}

void rainbow(uint32_t Frame, uint8_t from = 0, uint8_t to = NUM_LEDS, uint8_t deltahue = 5, uint8_t initialhue = 0)
{
    fill_rainbow(&leds[from], to - from, (initialhue + Frame * deltahue) % 255, deltahue);
}

void setup()
{
    Serial.begin(9600);

    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);

    FastLED.setBrightness(myLedState.brightness);
    fillColor(CRGB(myLedState.color));
    FastLED.show();
    delay(1000);
}

void loop()
{
    // put your main code here, to run repeatedly:

    if (myLedState.speed < currentSpeedVal)
    {
        currentSpeedVal = 0;

        switch (myLedState.currentAnimationId)
        {
        case 0:
            off();
            break;
        case 1:
            fillColor(CRGB(myLedState.color));
            FastLED.show();
            break;
        case 2:
            rainbow(currentFrame);
            currentFrame++;
            FastLED.show();
            break;
        }
    }
    else
    {
        currentSpeedVal++;
    }
}

void MQTTCallback(char *topic, byte *payload, unsigned int length)
{
}

void serialEvent()
{
    while (Serial.available())
    {
        // get the new byte:
        char inChar = (char)Serial.read();
        // add it to the inputString:
        //inputString += inChar;
        // if the incoming character is a newline, set a flag so the main loop can
        // do something about it:
        if (inChar == '\n')
        {
            //stringComplete = true;
        }
    }
}