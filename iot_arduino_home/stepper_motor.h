#ifndef STEPPERMOTOR_H
#define STEPPERMOTOR_H

#include <Stepper.h>

class StepperMotor {
  private:
    const int stepsPerRevolution = 2048;  // Numero di passi per una rivoluzione completa
    Stepper myStepper;                    // Oggetto Stepper
    
    float accumulatedError = 0.0;         // Errore accumulato dalla parte decimale

  public:
    StepperMotor(int pin1, int pin2, int pin3, int pin4);
    
    void setSpeed(long speed);
    void stepForward(int steps);
    void stepBackward(int steps);
    void rotate(float steps);
    
    void correctError();
};

#endif