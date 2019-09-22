#include <Arduino.h>
#include <FastLED.h>
#include "../../../contracts/basetypes.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//MQTT und Wlan:
const char *ssid = "Forum";
const char *password = "Hack2019";
const char *mqtt_server = "172.16.3.193";

WiFiClient espClient;
PubSubClient client(espClient);

void MQTTCallback(char *topic, byte *payload, unsigned int length);
void reconnect();

// Information about the LED strip itself
#define LED_PIN 3

#define NUM_LEDS 300

#define CHIPSET WS2811

#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];
CRGB fadeInColors[NUM_LEDS];
CRGB fadeOutColors[NUM_LEDS];

//current Animation variables:
uint32_t currentFrame = 0;

const char *mqttPath = "display/#";
unsigned long lastFrameTime = 0;

unsigned long lastPause = 0; //letzes mal das die Animation nicht ausgeführ wurde und keine Fehlerbehebung
unsigned long currentMillis = 0;
unsigned long fadeStartTime = 0;

bool fadeDone = true;

// Animations Variablen, die über MQTT aktuallisiert werden:
struct ledState myLedState;  //Led state used for animation
struct ledState newLedState; // over mqtt transmitted ledstate, used to update myLedState.

void loadDefaultLedState()
{
    myLedState.speed = (uint16_t)3000;
    myLedState.brightness = (uint8_t)128;
    myLedState.fadeId = (uint8_t)0;
    myLedState.fadeSpeed = (uint16_t)0;
    myLedState.animationId = (uint8_t)3;
    myLedState.color = (uint32_t)0xAA0000;
    myLedState.decay = (uint8_t)200;
    myLedState.colorRotation = (float)0;
    myLedState.animationSize = (uint16_t)10;
}

void printLedState()
{
    Serial.println("-------");
    Serial.print("brightness: ");
    Serial.println(myLedState.brightness, HEX);
    Serial.print("speed: ");
    Serial.println(myLedState.speed, HEX);
    Serial.print("fadeId: ");
    Serial.println(myLedState.fadeId, HEX);
    Serial.print("fadeSpeed: ");
    Serial.println(myLedState.fadeSpeed, HEX);
    Serial.print("AnimationId: ");
    Serial.println(myLedState.animationId, HEX);
    Serial.print("color (Hex): ");
    Serial.println(myLedState.color, HEX);
    Serial.print("decay: ");
    Serial.println(myLedState.decay, HEX);
    Serial.print("animationSize: ");
    Serial.println(myLedState.animationSize, HEX);
    Serial.print("colorRotation: ");
    Serial.println(myLedState.colorRotation);
    Serial.println("-------");
}

void finishFade()
{
    memcpy(&myLedState, &newLedState, sizeof(myLedState));
    fadeDone = true;
}

void fillColor(CRGB color, CRGB *target = leds, uint16_t from = 0, uint16_t to = NUM_LEDS)
{
    for (int i = from; i < to; i++)
    {
        target[i] = color;
    }
}

void off()
{
    fillColor(CRGB::Black);
}

void rainbow(uint32_t frame, CRGB *target = leds, uint16_t from = 0, uint16_t to = NUM_LEDS, uint8_t deltahue = 5, uint8_t initialhue = 0)
{
    fill_rainbow(&target[from], to - from, (initialhue + frame * deltahue) % 255, deltahue);
}

void knightRider(uint32_t frame, CRGB *target = leds, struct ledState pLedState = myLedState, uint16_t from = 0, uint16_t to = NUM_LEDS - 1)
{
    uint16_t temp = frame % (2 * (to - from));
    if (temp < to - from)
    { //Hinweg von from zu to
        //Serial.println(from + temp);
        target[from + temp] = pLedState.color;
    }
    else
    {
        //Serial.println(to - temp + to - from);
        target[to - temp + to - from] = pLedState.color;
    }
}

void strope(uint32_t frame, CRGB *target = leds, struct ledState pLedState = myLedState, uint16_t from = 0, uint16_t to = NUM_LEDS)
{
    if (frame % pLedState.animationSize == 0)
    {
        fillColor(pLedState.color, target, from, to);
    }
}

void setup_wifi(bool ledAnimation = true, int tries = 1000)
{

    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    for (int i = 0; i < tries && WiFi.status() != WL_CONNECTED; i++)
    {
        if (ledAnimation)
        {
            off();
            knightRider(i, leds, myLedState, 0, 99);
            knightRider(i, leds, myLedState, 100, 199);
            knightRider(i, leds, myLedState, 200, 299);
            FastLED.show();
        }
        delay(10);
        if (i % 30 == 0)
        {
            Serial.print(".");
        }
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup()
{
    loadDefaultLedState();
    Serial.begin(115200);
    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);

    FastLED.setBrightness(myLedState.brightness);

    fillColor(CRGB(myLedState.color));

    FastLED.show();

    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(MQTTCallback);
    reconnect();

    if (client.connected())
    {
        fillColor(0x00AA00, 0, 300);
        FastLED.show();
        delay(1000);
    }

    printLedState();
}

void decayArea(uint8_t dimVal, CRGB *target = leds, uint16_t from = 0, uint16_t to = NUM_LEDS)
{
    fadeToBlackBy(&target[from], to - from, dimVal);
}

void calculateLedColors(uint16_t frame, struct ledState pLedState = myLedState, CRGB *target = leds)
{
    switch (pLedState.animationId)
    {
    case 0:
        off();
        break;
    case 1:
        fillColor(CRGB(pLedState.color));

        break;
    case 2:
        rainbow(frame);

        break;
    case 3:

        knightRider(frame + NUM_LEDS);
        knightRider(frame);
        break;
    case 4:
        strope(frame);
        break;
    }
}

void fade(uint8_t frame)
{
    currentMillis = millis();
    if (currentMillis > fadeStartTime + newLedState.fadeSpeed)
    {
        finishFade();
    }
    else
    {

        decayArea(newLedState.decay, fadeInColors);
        decayArea(myLedState.decay, fadeOutColors);

        //TO-DO rotate color by colorRotation

        calculateLedColors(frame, newLedState, fadeInColors);
        calculateLedColors(frame, myLedState, fadeOutColors);

        blend(fadeOutColors, fadeInColors, leds, NUM_LEDS, (currentMillis - fadeStartTime) / newLedState.fadeSpeed);
    }
}

void loop()
{
    currentMillis = millis();
    if (!fadeDone)
    {
        if (currentMillis > lastFrameTime + newLedState.speed)
        {
            FastLED.show();
            lastFrameTime = millis();
            fade(currentFrame);
            
            currentFrame++;
        }
    }
    else
    {

        // next frame should be rendered and had time for potential error correction in the last 5s
        if (currentMillis > lastFrameTime + myLedState.speed && lastPause + 5000 > currentMillis)
        {
            FastLED.show(); //show last calculated Frame than calculate next frame
            lastFrameTime = millis();

            decayArea(myLedState.decay);

            if (myLedState.colorRotation != 0)
            {
                //TO-DO   This color transformation needs to be debuged
                Serial.print(myLedState.color, HEX);
                Serial.print(" -> ");
                myLedState.color = (uint32_t)(CRGB(myLedState.color) + CHSV(myLedState.colorRotation, 0xFF, 0xFF));
                Serial.print(myLedState.color, HEX);
            }

            calculateLedColors(currentFrame, myLedState);

            //FastLED.show();   first show than calculate to soomthen on Animation changes.
            currentFrame++;
        }
        else if (WiFi.status() != WL_CONNECTED)
        {
            fillColor(CRGB(0xFF0000), 0, 10); // setze die ersten 10 leds auf rot, wenn die Wlan verbindung unterbrochen ist.
            setup_wifi(false, 20);
        }
        else if (!client.connected())
        {
            reconnect();
        }
        else
        {
            lastPause = millis();
        }
    }
    client.loop();
}



void updateLEDState()
{

    // if colorotation is going throw all colors within 2min, ignore new color to avoide color jumps
    if (newLedState.colorRotation * 120000 > 255 * newLedState.speed)
    {
        newLedState.color = myLedState.color;
    }

    if (newLedState.fadeSpeed > 0)
    {
        fadeDone = false;
        fillColor(0, fadeInColors);
        memmove8(fadeOutColors, leds, NUM_LEDS * sizeof(CRGB));
        fadeStartTime = millis();
    }

    FastLED.setBrightness(myLedState.brightness);
}

void MQTTCallback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");

    Serial.print(" payload: ");
    for (int i = 0; i < length; i++)
    {
        Serial.print(payload[i], HEX);
        Serial.print(" ");
    }

    if (strncmp(topic, mqttPath, strlen(mqttPath) - 1) == 0)
    {
        if (strncmp(topic + strlen(mqttPath) - 1, "ledState", strlen("ledState")) == 0)
        {
            Serial.print(" ledState length: ");
            Serial.println(length);

            if (length != sizeof(myLedState))
            {
                Serial.print("struct length: ");
                Serial.print(sizeof(myLedState));
                Serial.print(" length of payload and struct length not equal!");
            }
            printLedState();
            if (!fadeDone)
            {
                finishFade();
            }
            memcpy(&newLedState, payload, min(sizeof(myLedState), length));

            updateLEDState();
            printLedState();
        }
    }
}

void reconnect()
{
    // Loop until we're reconnected

    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client"))
    {
        Serial.println("connected");
        // Once connected, publish an announcement...
        client.publish("display/online", "hello world");
        // ... and resubscribe
        client.subscribe(mqttPath);
    }
    else
    {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 1 second");
        // Wait 1 second before retrying
        //delay(1000);
    }
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
