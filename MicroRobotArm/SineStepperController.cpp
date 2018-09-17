/*
  SineStepperController library
  Author: T-Kuhn.
  Sapporo, October, 2018. Released into the public domain.
  */

#include "Constants.h"
#include "Arduino.h"
#include "SineStepper.h"
#include "SineStepperController.h"
#include "Queue.h"

// - - - - - - - - - - - - - - -
// - - - - CONSTRUCTOR - - - - -
// - - - - - - - - - - - - - - -
SineStepperController::SineStepperController(double freq)
{
    _frequency = freq;
    _counter = 0;
    isExecuting = false;

    for (uint8_t i = 0; i < MAX_NUM_OF_STEPPERS; i++)
    {
        _sineSteppers[i] = {0};
    }
}

// - - - - - - - - - - - - - - -
// - - - - - ATTACH  - - - - - -
// - - - - - - - - - - - - - - -
void SineStepperController::attach(SineStepper *sStepper)
{
    if (sStepper->id < MAX_NUM_OF_STEPPERS)
    {
        _sineSteppers[sStepper->id] = sStepper;
    }
}

// - - - - - - - - - - - - - - -
// - - - - SET MOVE BATCH  - - -
// - - - - - - - - - - - - - - -
void SineStepperController::setMoveBatch(MoveBatch *mb)
{
    for (uint8_t i = 0; i < MAX_NUM_OF_STEPPERS; i++)
    {
        if (mb->batch[i].isActive)
        {
            _sineSteppers[i]->setGoalPos(mb->batch[i].positon);
        }
    }
    isExecuting = true;
}

// - - - - - - - - - - - - - - -
// - - - - - UPDATE  - - - - - -
// - - - - - - - - - - - - - - -
void SineStepperController::update()
{
    if (isExecuting)
    {
        // GENERATE PULSES
        _counter++;
        double theta = _counter * _frequency;
        // cosine takes values form 0 ~ 2.
        double cosine = (cos(theta) + 1.0);

        for (uint8_t i = 0; i < MAX_NUM_OF_STEPPERS; i++)
        {
            if (_sineSteppers[i] != 0)
            {
                _sineSteppers[i]->update(cosine);
            }
        }

        if (theta > M_PI)
        {
            isExecuting = false;
            _counter = 0;
        }
    }
}
