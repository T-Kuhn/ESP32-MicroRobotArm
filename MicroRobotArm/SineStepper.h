/*
  SineStepper library
  Author: T-Kuhn.
  Sapporo, Octobre, 2018. Released into the public domain.
  */

#ifndef SineStepper_h
#define SineStepper_h
#include "Arduino.h"

// - - - - - - - - - - - - - - - - - - -
// - - - - - Encoder CLASS - - - - - - -
// - - - - - - - - - - - - - - - - - - -
class SineStepper
{
public:
  SineStepper(uint8_t pinStep, uint8_t pinDir, uint8_t stepperID);
  void update(double cosine);
  int32_t currentPos;
  int8_t id;
  int8_t goalPosition;

private:
  uint8_t pulseFromAmplitude(double ampl, double cosine);

  uint8_t _pinStep;
  uint8_t _pinDir;
  bool _isMovingCW;
};

#endif
