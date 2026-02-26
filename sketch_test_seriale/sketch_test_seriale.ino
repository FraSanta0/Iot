const int buttonPin = A5;
const int ledPressed = 9;
const int ledNotPressed = 8;

int lastState = HIGH;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPressed, OUTPUT);
  pinMode(ledNotPressed, OUTPUT);

  Serial.begin(9600);
  // Messaggio di avvio per debug
  Serial.println("SISTEMA_AVVIATO");
}

void loop() {
  // --- PARTE 1: LETTURA COMANDI IN ARRIVO (Dall'IoT) ---
  if (Serial.available() > 0) {
    // Leggi la stringa fino al carattere di nuova riga
    String command = Serial.readStringUntil('\n');
    command.trim(); // Rimuove spazi o caratteri invisibili

    // Logica di risposta ai comandi del Digital Twin
    if (command == "REQ") {
      // Il cervello chiede lo stato attuale
      if (digitalRead(buttonPin) == LOW) {
        Serial.println("Presa");
      } else {
        Serial.println("Non presa");
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
  int state = digitalRead(buttonPin);

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