/*
  ESP32 MicroRobotArm
  Author: T-Kuhn.
  Sapporo, October, 2018. Released into the public domain.
 */

#include "Constants.h"
#include "SineStepper.h"
#include "SineStepperController.h"
#include "Queue.h"
#include "MoveBatch.h"

SineStepper sineStepper1(/*pinStep:*/ 2, /*pinDir:*/ 4, /*id:*/ 0);
SineStepper sineStepper2(/*pinStep:*/ 19, /*pinDir:*/ 21, /*id:*/ 1);
SineStepperController sineStepperController(/*endlessRepeat:*/ true);

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile SemaphoreHandle_t timerSemaphore;

void IRAM_ATTR onTimer()
{
  digitalWrite(EXECUTING_ISR_CODE, HIGH);

  portENTER_CRITICAL_ISR(&timerMux);
  sineStepperController.update();
  portEXIT_CRITICAL_ISR(&timerMux);

  xSemaphoreGiveFromISR(timerSemaphore, NULL);
  digitalWrite(EXECUTING_ISR_CODE, LOW);
}

void setup()
{
  Serial.begin(115200);
  timerSemaphore = xSemaphoreCreateBinary();
  sineStepperController.attach(&sineStepper1);
  sineStepperController.attach(&sineStepper2);

  // initialize MoveBatches
  MoveBatch mb;
  mb.addMove(/*id:*/ 0, /*pos:*/ 70);
  mb.addMove(/*id:*/ 1, /*pos:*/ 9);
  mb.moveDuration = 1.0;
  sineStepperController.addMoveBatch(mb);

  mb.addMove(/*id:*/ 0, /*pos:*/ -130);
  mb.moveDuration = 1.0;
  sineStepperController.addMoveBatch(mb);

  mb.addMove(/*id:*/ 0, /*pos:*/ 256);
  mb.moveDuration = 1.0;
  sineStepperController.addMoveBatch(mb);

  mb.addMove(/*id:*/ 0, /*pos:*/ 0);
  mb.addMove(/*id:*/ 1, /*pos:*/ 0);
  mb.moveDuration = 1.0;
  sineStepperController.addMoveBatch(mb);

  pinMode(EXECUTING_ISR_CODE, OUTPUT);

  // Set 80 divider for prescaler
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  // onTimer gets called every 100uS.
  timerAlarmWrite(timer, 100, true);
  timerAlarmEnable(timer);
}

void loop()
{
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE)
  {
    int32_t pos = 0;

    portENTER_CRITICAL(&timerMux);
    pos = sineStepper1.currentPos;
    portEXIT_CRITICAL(&timerMux);

    Serial.print("pos: ");
    Serial.println(pos);
    delay(10);
  }
}

// MEMO:
//
// TODO:
//
// DOING:
//
// DONE:
// - automatic frequency calculation (calculated from highest stepsToTake)
//   -> settable moveBatchDuration felt more useful.
// - add repeat function.
// - make MoveBatch creation more convenient to use
// - make sure all moves end with the pulse output on LOW
// - flesh out implementation
//
