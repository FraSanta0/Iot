#include <ArduinoJson.h>
#include "stepper_motor.h"
#define MIN_LED_ON_TIME 2000

StepperMotor carousel_motor(8, 10, 9, 11);
struct JSONMessage {
    const char* target = nullptr;
    const char* command = nullptr;
    int val = -1;
    const char* sender = nullptr;
    const char* timestamp = nullptr;
};
enum State { WAITING_MESSAGE, PROCESS_MESSAGE, SEND_RESPONSE };
State current_state;
const int led = 7;
char msg[256];
bool isTaken = false;
StaticJsonDocument<256> doc;
unsigned long ledTimer = 0;
bool ledForcedOn = false; 

void sendError(int code, const char *msg);
bool getMessage(char *msg);
bool JSONDeserialize(char *msg, JSONMessage *fileds);
bool checkPill(struct JSONMessage *fields, const int lightSource, bool *isTaken);
void sendMessage(bool isTaken);

void setup() {
  Serial.begin(9600);

  carousel_motor.setSpeed(10);

  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  current_state = WAITING_MESSAGE;
}

void loop() {
  switch(current_state) {

    case WAITING_MESSAGE:
      if(getMessage(msg, sizeof(msg))){
        current_state = PROCESS_MESSAGE;
      }
      turnOffLed();
      break;

    case PROCESS_MESSAGE: {
      struct JSONMessage fields;
      if(!JSONDeserialize(msg, &fields)){
        current_state = WAITING_MESSAGE;
      }
      if(checkPill(&fields, led, &isTaken)){
        current_state = SEND_RESPONSE;
      }else{
        current_state = WAITING_MESSAGE;
      }
      turnOffLed();
      break;
    }

    case SEND_RESPONSE:
      sendMessage(isTaken);
      current_state = WAITING_MESSAGE;
      turnOffLed();
      break;
  }
}

void sendError(int code, const char* message) {

  StaticJsonDocument<200> err;

  err["status"] = "ERROR";
  err["code"] = code;
  err["message"] = message;
  err["sender"] = "home";

  serializeJson(err, Serial);
  Serial.println();
}

/*Ottiene il messaggio attraveerso bus seriale*/
bool getMessage(char *msg, size_t msg_lenght){
  if (Serial.available()) {
    size_t len = Serial.readBytesUntil('\n', msg, msg_lenght);
    if(len > msg_lenght - 1){
      sendError(102, "ERROR: Message Too long; discarged.\n");
      return false;
    }
    msg[len] = '\0';
    return true;
  }else{
    return false;
  }
}

/*Estrazione dei campi da parte del messaggio*/
bool JSONDeserialize(char *msg, JSONMessage *fields){
  DeserializationError error = deserializeJson(doc, msg);

  if (error) {
    sendError(101, "parsing JSON failed");
    return false;
  }

  fields->target=doc["target"];
  fields->command=doc["command"];
  fields->val=doc["val"];
  fields->sender=doc["sender"];
  fields->timestamp=doc["timestamp"];
  return true;
}

/*Logica di controllo sulla presa o meno della pillola*/
bool checkPill(struct JSONMessage *fields, const int lightSource, bool *isTaken){
  if(fields->target && fields->command && (strcmp(fields->target, "all") == 0 || strcmp(fields->target, "home") == 0) && strcmp(fields->command, "REQ") == 0){
    long sum = 0;
    size_t count = 0;
    turnOnLed();
    unsigned long startMillis = millis();
    while(millis() - startMillis < MIN_LED_ON_TIME){
      sum += analogRead(A0);
      count ++;
      delay(20);
    }
    *isTaken = (sum / count) > 512;
    return true;
  }else{
    return false;
  }
}

/*Accensione non bloccante del LED*/
void turnOnLed() {
  if (!ledForcedOn) {
    digitalWrite(led, HIGH);
    ledTimer = millis();  // registra l'istante dell'accensione
    ledForcedOn = true;
  }
}

/*Spegnimento non bloccante del LED*/
void turnOffLed() {
  if (ledForcedOn && millis() - ledTimer >= MIN_LED_ON_TIME) {
    digitalWrite(led, LOW);
    ledForcedOn = false;
  }
}

/*Creazione del messaggio JSON da inviare a nodeRED e di conseguenza ad Adafruit*/
void sendMessage(bool isTaken){
  doc.clear();
  doc["command"] = "RES";
  doc["command_response"] = isTaken ? "taken" : "not_taken";
  doc["sender"] = "home";
  serializeJson(doc, Serial);
  Serial.println();
}