/*
  SineStepperController library
  Author: T-Kuhn.
  Sapporo, Octobre, 2018. Released into the public domain.
  */

#include "Arduino.h"
#include "SineStepper.h"
#include "SineStepperController.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - SineStepperController CONSTRUCTOR - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - -
SineStepperController::SineStepperController(double freq)
{
    _frequency = freq;
    _numOfAttachedSteppers = 0;
    for (uint8_t i = 0; i < MAX_NUM_OF_STEPPERS; i++)
    {
        _sineSteppers[i] = {0};
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - SineStepperController ATTACH  - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - -
void SineStepperController::attach(SineStepper *sStepper)
{
    if (_numOfAttachedSteppers < MAX_NUM_OF_STEPPERS)
    {
        _sineSteppers[_numOfAttachedSteppers] = sStepper;
        _numOfAttachedSteppers++;
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - - SineStepperController UPDATE  - - - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - -
void SineStepperController::update(uint32_t isrCounter)
{
    // cosine takes values form 0 ~ 2.
    double cosine = (cos(isrCounter * _frequency) + 1.0);

    for (uint8_t i = 0; i < MAX_NUM_OF_STEPPERS; i++)
    {
        if (_sineSteppers[i] != 0)
        {
            _sineSteppers[i]->update(cosine);
        }
    }
}
