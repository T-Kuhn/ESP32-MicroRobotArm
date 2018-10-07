/*
  SineStepper library
  Author: T-Kuhn.
  Sapporo, October, 2018. Released into the public domain.
  */

#ifndef SineStepper_h
#define SineStepper_h
#include "Constants.h"
#include "Arduino.h"

class SineStepper
{
  public:
    SineStepper(uint8_t pinStep, uint8_t pinDir, uint8_t stepperID);
    void update(double cosine);
    void setGoalPos(int32_t goalPos);
    void setStepsToTakeToZero();
    int32_t currentPos;
    int8_t id;

  private:
    uint8_t pulseFromAmplitude(double stepsToTake, double cosine);
    int32_t _goalPosition;
    int32_t _currentStepsToTake;
    uint8_t _pinStep;
    uint8_t _pinDir;
    uint8_t _lastPulse;
    bool _isMovingCW;
};

#endif
