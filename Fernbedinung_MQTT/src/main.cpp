#include <Arduino.h>;
#include <Bounce2>;
#include <PubSubClient>

const int Pin_Auswahl_hoch = ;
const int Pin_Auswahl_runter = ;
const int Pin_Poti = ;
const int Pin_Zurueck = ;
const int Pin_Bestaetigen = ;

String Home[] = {"Modus", "Farbe","Schnelligkeit", "Aus"};
String Farbe[] = {"Rot", "Grün", "Blau", "Helligkeit"};
String Modus[] = {"Stroboskob", "Musik-Auswertung", "Night-Rider"};

Byte Uebergabeglobal = 0;
Byte Scroll[] == {{0,0}, {0,0}, {0,0}};

byte input() {// button Input
    byte Uebergabe  = 0;
    while (Uebergabe == 0) {
        switch (Uebergabe){
            case (digitalRead(Pin_Auswahl_hoch) == 1){
                Uebergabe = 1;
                break;
            }
            case (digitalRead(Pin_Auswahl_runter) == 1){
                Übergabe = 2;
                break;
            }
            case (digitalRead(Pin_Zurueck) == 1){
                Übergabe = 3;
                break;
            }
            case (digitalRead(Pin_Bestaetigen) == 1){
                Übergabe = 4
                break;
            }
        }
    }
    return Uebergabe;
}

void mqtt_out() {

}

void mqtt_in() {

}

void displayinfo(byte UebergabeIn) {

}

void berechnung(byte UebergabeIn){
    switch (var){
        case (UebergabeIn == 1){
            displayinfo(1);
            
            break;
        }
        case (UebergabeIn == 2){
            displayinfo(2);
            break;
        }
        case (UebergabeIn == 3){
            displayinfo(3);
            break;
        }
        case (UebergabeIn == 4){
            displayinfo(4);
            break;
        }
    }
}

void Potieinstellungen(){
    return analogRead(Pin_Poti)
}


void setup() {
    Serial.begin(9600)
    pinMode(Pin_Auswahl_hoch, INPUT);
    pinMode(Pin_Auswahl_runter, INPUT);
    pinMode(Pin_Poti, INPUT);
    pinMode(Pin_Zurück, INPUT);
    pinMode(Pin_Bestätigen, INPUT);
}

void loop() {
    Uebergabeglobal = input();
    berechnung(Uebergabeglobal);
}