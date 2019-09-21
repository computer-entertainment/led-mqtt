#include <Arduino.h>
#include <FastLED.h>
#include "../../../contracts/basetypes.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//MQTT und Wlan:
const char* ssid = "Forum";
const char* password = "Hack2019";
const char* mqtt_server = "172.16.3.193";

WiFiClient espClient;
PubSubClient client(espClient);



void setup_wifi(); 
void MQTTCallback(char *topic, byte *payload, unsigned int length);
void reconnect();

// Information about the LED strip itself
#define LED_PIN 3

#define NUM_LEDS 300

#define CHIPSET WS2811

#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

//current Animation variables:
uint32_t currentFrame = 0;
uint16_t currentSpeedVal = 0;

const char* mqttPath = "display/#";


// Animations Variablen, die über MQTT aktuallisiert werden:
struct ledState myLedState;


void loadDefaultLedState(){
    myLedState.speed = (uint16_t)3000;
    myLedState.brightness = (uint8_t) 128;
    myLedState.fadeId = (uint8_t) 0;
    myLedState.fadeSpeed = (uint16_t) 0;
    myLedState.animationId = (uint8_t) 3;
    myLedState.color = (uint32_t) 0xAA0000;
    myLedState.decay = (uint8_t) 200;
    myLedState.colorRotation = (float) 0;
    myLedState.animationSize = (uint16_t)10;
}

void printLedState()
{
    Serial.println("-------");
    Serial.print("brightness: ");
    Serial.println(myLedState.brightness,HEX);
    Serial.print("speed: ");
    Serial.println(myLedState.speed,HEX);
    Serial.print("fadeId: ");
    Serial.println(myLedState.fadeId,HEX);
    Serial.print("fadeSpeed: ");
    Serial.println(myLedState.fadeSpeed,HEX);
    Serial.print("AnimationId: ");
    Serial.println(myLedState.animationId,HEX);
    Serial.print("color (Hex): ");
    Serial.println(myLedState.color,HEX);
    Serial.print("decay: ");
    Serial.println(myLedState.decay,HEX);
    Serial.print("animationSize: ");
    Serial.println(myLedState.animationSize, HEX);
    Serial.print("colorRotation: ");
    Serial.println(myLedState.colorRotation);
    Serial.println("-------");
}

void fillColor(CRGB color, uint16_t from = 0, uint16_t to = NUM_LEDS)
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

void rainbow(uint32_t frame, uint16_t from = 0, uint16_t to = NUM_LEDS, uint8_t deltahue = 5, uint8_t initialhue = 0)
{
    fill_rainbow(&leds[from], to - from, (initialhue + frame * deltahue) % 255, deltahue);
}

void knightRider(uint32_t frame, uint16_t from = 0, uint16_t to = NUM_LEDS - 1){
    uint16_t temp = frame % (2 * (to - from));
    if(temp < to - from){   //Hinweg von from zu to
        //Serial.println(from + temp);
        leds[from + temp] = myLedState.color;
    }else{
        //Serial.println(to - temp + to - from);
        leds[to - temp + to - from] = myLedState.color;
    }
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


    if(client.connected()){
        fillColor(0x00AA00,0,300);
        FastLED.show();
        delay(1000);
    }
    
    printLedState();
    
}

void decayArea(uint8_t dimVal, uint16_t from = 0, uint16_t to = NUM_LEDS){
    fadeToBlackBy(&leds[from], to - from, dimVal);
    
}

void loop()
{

    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    // put your main code here, to run repeatedly:

    if (myLedState.speed < currentSpeedVal)
    {
        currentSpeedVal = 0;
        decayArea(myLedState.decay);

        switch (myLedState.animationId)
        {
        case 0:
            off();
            break;
        case 1:
            fillColor(CRGB(myLedState.color));

            break;
        case 2:
            rainbow(currentFrame);

            break;
        case 3:
            
            knightRider(currentFrame + NUM_LEDS);
            knightRider(currentFrame);
        break;
        case 4;

        break;
        }

        FastLED.show();
        currentFrame++;
    }
    else
    {
        currentSpeedVal++;
    }
}

void updateLEDState(){
    FastLED.setBrightness(myLedState.brightness);
}

void MQTTCallback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");

    Serial.print(" payload: ");
    for(int i = 0; i < length; i++){
        Serial.print(payload[i],HEX);
        Serial.print(" ");
    }

    if(strncmp(topic, mqttPath, strlen(mqttPath)-1) == 0){
        if(strncmp(topic + strlen(mqttPath)-1, "ledState", strlen("ledState")) == 0){
            Serial.print(" ledState length: ");
            Serial.println(length);
            
            if(length != sizeof(myLedState))
            {
                Serial.print("struct length: ");
                Serial.print(sizeof(myLedState));
                Serial.print(" length of payload and struct length not equal!");
            }
            printLedState();
            memcpy(&myLedState, payload, min(sizeof(myLedState), length));
            printLedState();
            updateLEDState();
        }
    }

}



void reconnect() {
    // Loop until we're reconnected
    
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (client.connect("ESP8266Client")) {
        Serial.println("connected");
        // Once connected, publish an announcement...
        client.publish("display/online", "hello world");
        // ... and resubscribe
        client.subscribe(mqttPath);
      } else {
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

void setup_wifi() {
    
      // We start by connecting to a WiFi network
      Serial.println();
      Serial.print("Connecting to ");
      Serial.println(ssid);
    
      WiFi.begin(ssid, password);
    
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
    
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
    }