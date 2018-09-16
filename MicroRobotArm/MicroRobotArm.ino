/*
  ESP32 MicroRobotArm
  Author: T-Kuhn.
  Sapporo, Octobre, 2018. Released into the public domain.
 */

#include "SineStepper.h"
#include "SineStepperController.h"

#define EXECUTING_ISR_CODE 5

SineStepper sineStepper1(/*pinStep:*/ 2, /*pinDir:*/ 4, /*id:*/ 0);
SineStepper sineStepper2(/*pinStep:*/ 19, /*pinDir:*/ 21, /*id:*/ 1);
SineStepperController sineStepperController(/*frequency:*/ 0.0005);

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile SemaphoreHandle_t timerSemaphore;
volatile uint32_t isrCounter = 0;

void IRAM_ATTR onTimer()
{
  digitalWrite(EXECUTING_ISR_CODE, HIGH);

  portENTER_CRITICAL_ISR(&timerMux);
  isrCounter++;
  sineStepperController.update(isrCounter);
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
    delay(10);
  }
}

// MEMO:
//
// TODO:
//
// DOING:
// - implement "SineStepper"
// - implement "SineStepperController"
// - implement "SineMoveBatch"
//
// DONE:
// - get that encoder library in here (just to see how it's done.)
// - implement a mechanism to see how much time we spend in the ISR.
// - implement that sin thing.
//
