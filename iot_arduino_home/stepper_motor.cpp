#include "stepper_motor.h"

// Costruttore: inizializza l'oggetto Stepper con i pin passati
StepperMotor::StepperMotor(int pin1, int pin2, int pin3, int pin4)
  : myStepper(stepsPerRevolution, pin1, pin2, pin3, pin4) {
  // Il costruttore configura il motore con i pin dati
}

void StepperMotor::setSpeed(long speed) {
  myStepper.setSpeed(speed);
}

void StepperMotor::stepForward(int steps) {
  myStepper.step(steps);
}

void StepperMotor::stepBackward(int steps) {
  myStepper.step(-steps);
}

// Metodo che permette di ruotare di un numero di passi (anche decimali)
void StepperMotor::rotate(float steps) {
  int stepsToMove = int(steps);  // La parte intera dei passi da fare
  accumulatedError = steps - stepsToMove;  // Calcola la parte decimale dell'errore
  
  myStepper.step(stepsToMove);  // Fai il movimento con la parte intera

  // Correggi l'errore accumulato
  correctError();
}

// Metodo che corregge l'errore accumulato, aggiungendo o sottraendo passi
void StepperMotor::correctError() {
  // Correggi se l'errore è maggiore o uguale a 1
  if (accumulatedError >= 1.0) {
    myStepper.step(1);  // Aggiungi un passo in eccesso
    accumulatedError -= 1.0;  // Resetta l'errore
  }
  // Correggi se l'errore è minore o uguale a -1
  else if (accumulatedError <= -1.0) {
    myStepper.step(-1);  // Sottrai un passo
    accumulatedError += 1.0;  // Resetta l'errore
  }
}