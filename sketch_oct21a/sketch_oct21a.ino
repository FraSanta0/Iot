#include <Servo.h>
Servo myservo;
int servoPin = 9;
int sensorPin = A0;
void setup() {
Serial.begin(9600);
myservo.attach(servoPin);
Serial.println("Arduino ready");
}
void loop() {
if (Serial.available() > 0) {
String command = Serial.readStringUntil('\n');
command.trim();
if (command == "ON") {
myservo.write(90); // open irrigation
Serial.println("Irrigation ON");
} else if (command == "OFF") {
myservo.write(0); // close irrigation
Serial.println("Irrigation OFF");
}
}
int sensorValue = analogRead(sensorPin);
Serial.print("Moisture: ");
Serial.println(sensorValue);
delay(2000);
}