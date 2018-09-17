/*
  SineStepper library
  Author: T-Kuhn.
  Sapporo, October, 2018. Released into the public domain.
  */

#include "Arduino.h"
#include "SineStepper.h"
#include "Constants.h"

// - - - - - - - - - - - - - - -
// - - - - CONSTRUCTOR - - - - -
// - - - - - - - - - - - - - - -
SineStepper::SineStepper(uint8_t pinStep, uint8_t pinDir, uint8_t stepperID)
{
    currentPos = 0;
    id = stepperID;

    _pinStep = pinStep;
    _pinDir = pinDir;
    _isMovingCW = true;
    _goalPosition = 0;
    _currentStepsToTake = 0;

    pinMode(_pinStep, OUTPUT);
    pinMode(_pinDir, OUTPUT);
}

// - - - - - - - - - - - - - - -
// - - - - - UPDATE  - - - - - -
// - - - - - - - - - - - - - - -
void SineStepper::update(double cosine)
{
    uint8_t pulse = pulseFromAmplitude(_currentStepsToTake, cosine);
    digitalWrite(_pinStep, pulse);
}

// - - - - - - - - - - - - - - -
// - - - - SET GOAL POS  - - - -
// - - - - - - - - - - - - - - -
void SineStepper::setGoalPos(int32_t goalPos)
{
    _goalPosition = goalPos;
    _currentStepsToTake = goalPos - currentPos;
}

// - - - - - - - - - - - - - - -
// - -  PULSE FROM AMPLITUDE - -
// - - - - - - - - - - - - - - -
uint8_t SineStepper::pulseFromAmplitude(double ampl, double cosine)
{
    uint32_t doubledStepCount = (uint32_t)(cosine * ampl);
    uint32_t stepLevel = doubledStepCount % 2;

    if (stepLevel > digitalRead(_pinStep))
    {
        currentPos += _isMovingCW ? 1 : -1;
    }

    return stepLevel;
}
