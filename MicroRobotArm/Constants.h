/*
  Constants
  Author: T-Kuhn.
  Sapporo, October, 2018. Released into the public domain.
  */

#ifndef Constants_h
#define Constants_h
#include "Arduino.h"

#define EXECUTING_ISR_CODE 23
#define BUTTON_PIN 34
#define ROTARY_ENC_1_A 32
#define ROTARY_ENC_1_B 33
#define ROTARY_ENC_2_A 25
#define ROTARY_ENC_2_B 26
// NOTE: SineStepper and MoveBatch ids must be lower then MAX_NUM_OF_STEPPERS
#define MAX_NUM_OF_STEPPERS 10
#define MAX_NUM_OF_BATCHED_MOVES 100
#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923

#endif
