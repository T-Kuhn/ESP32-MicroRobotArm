/*
  SineStepperController library
  Author: T-Kuhn.
  Sapporo, October, 2018. Released into the public domain.
  */

#define MAX_NUM_OF_STEPPERS 10

#ifndef SineStepperController_h
#define SineStepperController_h

#include "Arduino.h"
#include "SineStepper.h"

// - - - - - - - - - - - - - - - - - - -
// - - - - - Encoder CLASS - - - - - - -
// - - - - - - - - - - - - - - - - - - -
class SineStepperController
{
public:
  SineStepperController(double freq);
  void update(uint32_t isrCounter);
  void attach(SineStepper *sStepper);

private:
  SineStepper *_sineSteppers[MAX_NUM_OF_STEPPERS];
  double _frequency;
  uint8_t _numOfAttachedSteppers;
};

#endif
