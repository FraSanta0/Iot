#include <ArduinoJson.h>

const int buttonPin = A5;
const int ledPressed = 9;
const int ledNotPressed = 8;

const int PIN_MONDAY = A0;
const int PIN_TUESDAY = A1;
const int PIN_WEDNESDAY = A2;
const int PIN_THURSDAY = A3;
const int PIN_FRIDAY = A4;


int lastState = HIGH;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPressed, OUTPUT);
  pinMode(ledNotPressed, OUTPUT);

  pinMode(PIN_MONDAY, INPUT_PULLUP);
  pinMode(PIN_TUESDAY, INPUT_PULLUP);
  pinMode(PIN_WEDNESDAY, INPUT_PULLUP);
  pinMode(PIN_THURSDAY, INPUT_PULLUP);
  pinMode(PIN_FRIDAY, INPUT_PULLUP);

  

  Serial.begin(9600);
  // Messaggio di avvio per debug
  Serial.println("SISTEMA_AVVIATO");
}

void loop() {
  // --- PARTE 1: LETTURA COMANDI IN ARRIVO (Dall'IoT) ---
  if (Serial.available() > 0) {

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, Serial);
    if(error) return;

    String command = doc["command"].as<String>(); // Es: "REQ"
    int val = doc["val"];
    command.trim();


    // Logica di risposta ai comandi del Digital Twin
    if (command=="REQ") {

      int pinToCheck;

      switch (val) {
        case 0: pinToCheck = PIN_MONDAY; break;
        case 1: pinToCheck = PIN_TUESDAY; break;
        case 2: pinToCheck = PIN_WEDNESDAY; break;
        case 3: pinToCheck = PIN_THURSDAY; break;
        case 4: pinToCheck = PIN_FRIDAY; break;
        
        default: pinToCheck = -1; break;
      }

      
      if (pinToCheck != -1) {
        int state = digitalRead(pinToCheck);
        if (state == LOW) {
          Serial.println("Presa");
          //Serial.println("{\"status\":\"PRESA\", \"day\":" + String(val) + "}");
        } else {
          Serial.println("Non presa");
          //Serial.println("{\"status\":\"NON_PRESA\", \"day\":" + String(val) + "}");
        }
      }
    } 
    else if (command == "ALM_ON") {
      // Esempio: far lampeggiare i LED per allarme
      digitalWrite(ledPressed, HIGH);
      digitalWrite(ledNotPressed, HIGH);
    }
    else if (command == "ALM_OFF") {
      // Ripristina lo stato normale
      digitalWrite(ledPressed, LOW);
      digitalWrite(ledNotPressed, HIGH);
    }
  }

  // --- PARTE 2: LOGICA LOCALE (Bottone) ---
  int state = digitalRead(PIN_THURSDAY);

  if (state == LOW) {
    digitalWrite(ledPressed, HIGH);
    digitalWrite(ledNotPressed, LOW);

    if (lastState != LOW) {
      Serial.println("Presa"); // Cambiato in "Presa" per coerenza con lo script Python
      lastState = LOW;
    }
  } else {
    digitalWrite(ledPressed, LOW);
    digitalWrite(ledNotPressed, HIGH);

    if (lastState != HIGH) {
      Serial.println("Non presa");
      lastState = HIGH;
    }
  }

  delay(20);
}