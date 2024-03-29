#include <Arduino.h>
#include <FastLED.h>
#include "../../../contracts/basetypes.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "../../../../config.h"    //this file will be outside the repository when cloning
// you can checkout how the config.h file is structured in exampleconfig.h and include it as following:
// #include "../../../exampleconfig.h" 

// Wlan configuration loaded from config.h:
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

// MQTT configuration loaded from config.h
const char *mqtt_server = MQTT_SERVER_IP;
const char *mqtt_user = MQTT_USER_LEDSTRIP;
const char *mqtt_password = MQTT_PASSWORD_LEDSTRIP;


const char *mqtt_clientID = "ESP8266ClientLED_Strip";

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
CRGB currentColor = CRGB(0);


const char *mqttPath = "display/#";
unsigned long lastFrameTime = 0;

unsigned long lastPause = 0; //letzes mal das die Animation nicht ausgeführ wurde und keine Fehlerbehebung
unsigned long currentMillis = 0;
unsigned long fadeStartTime = 0;

uint8_t colorOffset = 0;

bool fadeDone = true;

// Animations Variablen, die über MQTT aktuallisiert werden:
struct ledState myLedState;  //Led state used for animation
struct ledState newLedState; // over mqtt transmitted ledstate, used to update myLedState.

void loadDefaultLedState()
{
    myLedState.speed = (uint16_t)100;
    myLedState.brightness = (uint8_t)128;
    myLedState.fadeId = (uint8_t)0;
    myLedState.fadeSpeed = (uint16_t)0;
    myLedState.animationId = (uint8_t)3;
    myLedState.color = (uint32_t)0xAA0000;
    myLedState.decay = (uint8_t)200;
    myLedState.colorRotation = (float)1;
    myLedState.animationSize = (uint16_t)10;
    myLedState.colorRotationEffect = (uint8_t)20;
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
    Serial.print("colorRotationEffect: ");
    Serial.println(myLedState.colorRotationEffect);
    Serial.println("-------");
}

void finishFade()
{
    memcpy(&myLedState, &newLedState, sizeof(myLedState));
    fadeDone = true;
}

void fillColor(CRGB *target = leds, struct ledState pLedState = myLedState, uint16_t from = 0, uint16_t to = NUM_LEDS)
{
    for (int i = from; i < to; i++)
    {
        target[i] = currentColor;
    }
}

void off()
{
    fill_solid(&leds[0], NUM_LEDS, CRGB(0));
}

void rainbow(uint32_t frame, CRGB *target = leds, uint16_t from = 0, uint16_t to = NUM_LEDS, uint8_t deltahue = 5, uint8_t initialhue = 0)
{
    fill_rainbow(&target[from], to - from, (colorOffset + frame * deltahue) % 255, deltahue);
}

void knightRider(uint32_t frame, CRGB *target = leds, struct ledState pLedState = myLedState, uint16_t from = 0, uint16_t to = NUM_LEDS - 1)
{
    uint16_t temp = frame % (2 * (to - from));
    if (temp < to - from)
    { //Hinweg von from zu to
        //Serial.println(from + temp);
        target[from + temp] = currentColor;
    }
    else
    {
        //Serial.println(to - temp + to - from);
        target[to - temp + to - from] = currentColor;
    }
}

void strope(uint32_t frame, CRGB *target = leds, struct ledState pLedState = myLedState, uint16_t from = 0, uint16_t to = NUM_LEDS)
{
    if (frame % pLedState.animationSize == 0)
    {
        fillColor(target, pLedState, from, to);
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

    fillColor(leds, myLedState);

    FastLED.show();

    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(MQTTCallback);
    reconnect();

    if (client.connected())
    {
        fill_solid(leds, NUM_LEDS, 0x00AA00);
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
    currentColor = CRGB(pLedState.color);

    nblend(currentColor, CRGB(CHSV(colorOffset, 255, 255)), (fract8) pLedState.colorRotationEffect);

    
    switch (pLedState.animationId)
    {
    case 0:
        off();
        FastLED.show();
        break;
    case 1:
        fillColor(target, pLedState);
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
        
        Serial.print("time diff: ");
        Serial.println(currentMillis - fadeStartTime);
        float fadeRatio = (float) (currentMillis - fadeStartTime) / newLedState.fadeSpeed;

        Serial.print("fadeRatio: ");
        Serial.println(fadeRatio);
        blend(fadeInColors, fadeOutColors, leds, NUM_LEDS, fadeRatio * 255);

    }
}

uint32_t getColorCode(CRGB col){
    uint32_t retval = 0;
    memcpy(&retval, &col.raw, 3);
    
    return retval;
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
            FastLED.show();
            currentFrame++;
        }
    }
    else
    {

        // next frame should be rendered and had time for potential error correction in the last 5s
        if ((currentMillis > lastFrameTime + myLedState.speed || currentMillis < lastFrameTime )&& lastPause + 5000 > currentMillis )
        {
            FastLED.show(); //show last calculated Frame than calculate next frame
            lastFrameTime = millis();

            decayArea(myLedState.decay);

            //getColorCode(CRGB(CRGB(myLedState.color) + CHSV(myLedState.colorRotation, 0xFF, 0xFF)));

            if (myLedState.colorRotation != 0)
            {
                colorOffset = (uint8_t) (myLedState.colorRotation * currentFrame) %255;
            }
            else{
                colorOffset = 0;
            }

            calculateLedColors(currentFrame, myLedState);

            //FastLED.show();   first show than calculate to soomthen on Animation changes.
            currentFrame++;
        }
        else if (WiFi.status() != WL_CONNECTED)
        {
            fill_solid(leds, 10, CRGB(0xFF0000)); // setze die ersten 10 leds auf rot, wenn die Wlan verbindung unterbrochen ist.
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
        colorOffset = 0;
    }

    if (newLedState.fadeSpeed > 0)
    {
        fadeDone = false;
        fill_solid(fadeInColors, NUM_LEDS, 0);
        memmove8(fadeOutColors, leds, NUM_LEDS * sizeof(CRGB));
        fadeStartTime = millis();
    }
    else{
        finishFade();
    }
    FastLED.setBrightness(myLedState.brightness);
}

void MQTTCallback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");

    Serial.print(" payload: ");
    for (unsigned int i = 0; i < length; i++)
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
    boolean connectionResult = false;
    if(strncmp(mqtt_password, "", 1) == 0 && strncmp(mqtt_user, "", 1) == 0){
        connectionResult = client.connect(mqtt_clientID);
    }
    else{
        connectionResult = client.connect(mqtt_clientID, mqtt_user, mqtt_password);
    }

    if (connectionResult)
    {
        Serial.println("connected");
        // Once connected, publish an announcement...
        // client.publish("display/online", "hello world");
        // ... and resubscribe
        client.subscribe(mqttPath);
    }
    else
    {
        Serial.print("failed, rc=");
        Serial.print(client.state());
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
