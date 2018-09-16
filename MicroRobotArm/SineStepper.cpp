/*
  SineStepper library
  Author: T-Kuhn.
  Sapporo, Octobre, 2018. Released into the public domain.
  */

#include "Arduino.h"
#include "SineStepper.h"

// - - - - - - - - - - - - - - - - - - - - -
// - - - - SineStepper CONSTRUCTOR - - - - -
// - - - - - - - - - - - - - - - - - - - - -
SineStepper::SineStepper(uint8_t pinStep, uint8_t pinDir, uint8_t stepperID)
{
    currentPos = 0;
    id = stepperID;

    _pinStep = pinStep;
    _pinDir = pinDir;
    _isMovingCW = false;

    pinMode(_pinStep, OUTPUT);
    pinMode(_pinDir, OUTPUT);
}

// - - - - - - - - - - - - - - - - - - - - -
// - - - - - SineStepper UPDATE  - - - - - -
// - - - - - - - - - - - - - - - - - - - - -
void SineStepper::update(double cosine)
{
    uint8_t pulse = pulseFromAmplitude(1000.0, cosine);
    digitalWrite(_pinStep, pulse);
}

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
