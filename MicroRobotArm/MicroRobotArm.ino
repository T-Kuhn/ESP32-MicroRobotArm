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
#include "RobotArmIK.h"
#include "Encoder.h"

Encoder Encoder1(ROTARY_ENC_1_A, ROTARY_ENC_1_B);
Encoder Encoder2(ROTARY_ENC_2_A, ROTARY_ENC_2_B);

SineStepper sineStepper1(/*pinStep:*/ 2, /*pinDir:*/ 4, /*id:*/ 0);
SineStepper sineStepper2(/*pinStep:*/ 16, /*pinDir:*/ 17, /*id:*/ 1);
SineStepper sineStepper3(/*pinStep:*/ 5, /*pinDir:*/ 18, /*id:*/ 2);
SineStepper sineStepper4(/*pinStep:*/ 19, /*pinDir:*/ 21, /*id:*/ 3);
SineStepperController sineStepperController(/*endlessRepeat:*/ true);
RobotArmIK robotArmIK(43.0, 60.0, 70.0, 54.0);

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

    pinMode(EXECUTING_ISR_CODE, OUTPUT);
    pinMode(EXECUTING_ISR_CODE, OUTPUT);

    timerSemaphore = xSemaphoreCreateBinary();
    sineStepperController.attach(&sineStepper1);
    sineStepperController.attach(&sineStepper2);
    sineStepperController.attach(&sineStepper3);
    sineStepperController.attach(&sineStepper4);

    // initialize MoveBatches
    MoveBatch mb;
    //mb.addMove(/*id:*/ 0, /*pos:*/ 0);
    //mb.addMove(/*id:*/ 1, /*pos:*/ 0);
    //mb.addMove(/*id:*/ 2, /*pos:*/ 0);
    //mb.addMove(/*id:*/ 3, /*pos:*/ 0);
    //mb.moveDuration = 1;
    //sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.RunIK(114.0, 20.0, 0.0, mb);
    mb.moveDuration = 1.5;
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.RunIK(84.0, 20.0, 0.0, mb);
    mb.addMove(/*id:*/ 3, /*pos:*/ 1024);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.RunIK(114.0, 20.0, 0.0, mb);
    mb.addMove(/*id:*/ 3, /*pos:*/ 0);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.RunIK(97.0, 20.0, 0.5, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.RunIK(97.0, 20.0, -0.5, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.RunIK(94.0, 20.0, 0.0, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.RunIK(94.0, 50.0, 0.0, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.RunIK(134.0, 50.0, M_PI_2, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.RunIK(134.0, 80.0, M_PI_2, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.RunIK(134.0, 50.0, M_PI_2, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.RunIK(134.0, 50.0, M_PI_2 + 0.3, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.RunIK(134.0, 50.0, M_PI_2 - 0.3, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.RunIK(134.0, 94.0, M_PI_2 + 0.3, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.RunIK(134.0, 94.0, M_PI_2 - 0.3, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.RunIK(134.0, 94.0, M_PI_2, mb);
    mb.addMove(/*id:*/ 3, /*pos:*/ 0);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.RunIK(144.0, 94.0, M_PI_2, mb);
    mb.addMove(/*id:*/ 3, /*pos:*/ 1024);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.RunIK(124.0, 94.0, M_PI_2, mb);
    mb.addMove(/*id:*/ 3, /*pos:*/ 0);
    sineStepperController.addMoveBatch(mb);

    mb.addMove(/*id:*/ 0, /*pos:*/ 0);
    mb.addMove(/*id:*/ 1, /*pos:*/ 0);
    mb.addMove(/*id:*/ 2, /*pos:*/ 0);
    mb.addMove(/*id:*/ 3, /*pos:*/ 0);
    sineStepperController.addMoveBatch(mb);
    sineStepperController.addMoveBatch(mb);

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

        //Serial.print("pos: ");
        //Serial.println(pos);
        delay(10);
    }
}

// MEMO:
//
// TODO:
// - calculate max possible stepsToTake without loosing steps.
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
