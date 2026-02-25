const int buttonPin = A5;   // oppure D2 se preferisci
const int ledPressed = 9;   // LED acceso quando premuto
const int ledNotPressed = 8; // LED acceso quando non premuto

int lastState = HIGH; // per evitare spam seriale

void setup() {
  pinMode(buttonPin, INPUT_PULLUP); // bottone verso GND
  pinMode(ledPressed, OUTPUT);
  pinMode(ledNotPressed, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  int state = digitalRead(buttonPin);

  if (state == LOW) {
    // Bottone premuto
    digitalWrite(ledPressed, HIGH);
    digitalWrite(ledNotPressed, LOW);

    // Evita di mandare continuamente lo stesso messaggio
    if (lastState != LOW) {
      Serial.println("OCCUPATO");
      lastState = LOW;
    }

  } else {
    // Bottone non premuto
    digitalWrite(ledPressed, LOW);
    digitalWrite(ledNotPressed, HIGH);

    if (lastState != HIGH) {
      Serial.println("LIBERO");
      lastState = HIGH;
    }
  }

  delay(20); // debounce semplice
}
