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
SineStepperController sineStepperController(/*frequency:*/ 0.0005);

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
  MoveBatch batch0;
  MoveBatch batch1;
  MoveBatch batch2;
  MoveBatch batch3;
  batch0.addMove(/*id:*/ 0, /*pos:*/ 10);
  batch0.addMove(/*id:*/ 1, /*pos:*/ 20);
  batch1.addMove(/*id:*/ 0, /*pos:*/ 100);
  batch1.addMove(/*id:*/ 1, /*pos:*/ 200);
  batch2.addMove(/*id:*/ 0, /*pos:*/ 200);
  batch2.addMove(/*id:*/ 1, /*pos:*/ 400);
  batch3.addMove(/*id:*/ 0, /*pos:*/ 300);
  batch3.addMove(/*id:*/ 1, /*pos:*/ 500);
  portENTER_CRITICAL(&timerMux);
  sineStepperController.addMoveBatch(batch0);
  sineStepperController.addMoveBatch(batch1);
  sineStepperController.addMoveBatch(batch2);
  sineStepperController.addMoveBatch(batch3);
  portEXIT_CRITICAL(&timerMux);

  pinMode(EXECUTING_ISR_CODE, OUTPUT);

  // Set 80 divider for prescaler
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 512, true);
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
    delay(100);
  }
}

// MEMO:
//
// TODO:
// - implement direction
//
// DOING:
// - flesh out implementation
//
// DONE:
// - implement "SineMoveBatch"
// - implement "SineStepper"
// - implement "SineStepperController"
// - get that encoder library in here (just to see how it's done.)
// - implement a mechanism to see how much time we spend in the ISR.
// - implement that sin thing.
//
