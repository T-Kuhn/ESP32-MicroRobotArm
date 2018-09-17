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
SineStepperController sineStepperController(/*frequency:*/ 0.0001);

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
  batch0.addMove(/*id:*/ 0, /*pos:*/ 7);
  batch0.addMove(/*id:*/ 1, /*pos:*/ 9);
  sineStepperController.addMoveBatch(batch0);
  MoveBatch batch1;
  batch1.addMove(/*id:*/ 0, /*pos:*/ -13);
  batch1.addMove(/*id:*/ 1, /*pos:*/ -27);
  sineStepperController.addMoveBatch(batch1);
  MoveBatch batch2;
  batch2.addMove(/*id:*/ 0, /*pos:*/ 20);
  batch2.addMove(/*id:*/ 1, /*pos:*/ 30);
  sineStepperController.addMoveBatch(batch2);
  MoveBatch batch3;
  batch3.addMove(/*id:*/ 0, /*pos:*/ 0);
  batch3.addMove(/*id:*/ 1, /*pos:*/ 0);
  sineStepperController.addMoveBatch(batch3);

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
// - automatic frequency calculation (calculated from highest stepsToTake)
// - make MoveBatch creation more convenient to use
//
// DOING:
//
// DONE:
// - make sure all moves end with the pulse output on LOW
// - flesh out implementation
//
