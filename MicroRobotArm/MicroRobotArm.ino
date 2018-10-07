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

enum Mode
{
    adjustingJoint1,
    adjustingJoint2,
    adjustingJoint3,
    adjustingJoint4,
    doingControlledMovements
};

Mode currentMode = adjustingJoint1;

Encoder Encoder1(ROTARY_ENC_1_A, ROTARY_ENC_1_B);

SineStepper sineStepper1(STEPPER1_STEP_PIN, STEPPER1_DIR_PIN, /*id:*/ 0);
SineStepper sineStepper2(STEPPER2_STEP_PIN, STEPPER2_DIR_PIN, /*id:*/ 1);
SineStepper sineStepper3(STEPPER3_STEP_PIN, STEPPER3_DIR_PIN, /*id:*/ 2);
SineStepper sineStepper4(STEPPER4_STEP_PIN, STEPPER4_DIR_PIN, /*id:*/ 3);
SineStepperController sineStepperController(/*endlessRepeat:*/ true);
RobotArmIK robotArmIK(43.0, 60.0, 70.0, 54.0);

int buttonCoolDownCounter = 0;
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile SemaphoreHandle_t timerSemaphore;

void handleModeChange(Mode newMode)
{
    if (buttonCoolDownCounter < BUTTON_COOLDOWN_CYCLES)
    {
        buttonCoolDownCounter++;
    }
    if (digitalRead(BUTTON_PIN) && buttonCoolDownCounter >= BUTTON_COOLDOWN_CYCLES)
    {
        buttonCoolDownCounter = 0;
        currentMode = newMode;
    }
}

void IRAM_ATTR onTimer()
{
    digitalWrite(EXECUTING_ISR_CODE, HIGH);

    switch (currentMode)
    {
    case adjustingJoint1:
        Encoder1.update();
        digitalWrite(STEPPER1_DIR_PIN, Encoder1.currentRot);
        digitalWrite(STEPPER1_STEP_PIN, Encoder1.count % 2);

        handleModeChange(adjustingJoint2);
        break;
    case adjustingJoint2:
        Encoder1.update();
        digitalWrite(STEPPER2_DIR_PIN, Encoder1.currentRot);
        digitalWrite(STEPPER2_STEP_PIN, Encoder1.count % 2);

        handleModeChange(adjustingJoint3);
        break;
    case adjustingJoint3:
        Encoder1.update();
        digitalWrite(STEPPER3_DIR_PIN, Encoder1.currentRot);
        digitalWrite(STEPPER3_STEP_PIN, Encoder1.count % 2);

        handleModeChange(adjustingJoint4);
        break;
    case adjustingJoint4:
        Encoder1.update();
        digitalWrite(STEPPER4_DIR_PIN, Encoder1.currentRot);
        digitalWrite(STEPPER4_STEP_PIN, Encoder1.count % 2);

        handleModeChange(doingControlledMovements);
        break;
    case doingControlledMovements:
        portENTER_CRITICAL_ISR(&timerMux);
        sineStepperController.update();
        portEXIT_CRITICAL_ISR(&timerMux);
        break;
    default:
        break;
    }
    xSemaphoreGiveFromISR(timerSemaphore, NULL);
    digitalWrite(EXECUTING_ISR_CODE, LOW);
}

void setup()
{
    Serial.begin(115200);

    pinMode(EXECUTING_ISR_CODE, OUTPUT);
    pinMode(BUTTON_PIN, INPUT);

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

    mb.moveDuration = 1.7;

    //mb = robotArmIK.runIK(84.0, 20.0, 0.0, mb);
    //mb.addMove(/*id:*/ 3, /*pos:*/ 1024);
    //sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(5.0, 180.0, 0.0, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(134.0, 50.0, M_PI_2, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(134.0, 80.0, M_PI_2, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(134.0, 50.0, M_PI_2, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(134.0, 50.0, M_PI_2 + 0.3, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(134.0, 50.0, M_PI_2 - 0.3, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(134.0, 94.0, M_PI_2 + 0.3, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(134.0, 94.0, M_PI_2 - 0.3, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(134.0, 94.0, M_PI_2, mb);
    mb.addMove(/*id:*/ 3, /*pos:*/ 0);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(144.0, 94.0, M_PI_2, mb);
    mb.addMove(/*id:*/ 3, /*pos:*/ 1024);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(124.0, 94.0, M_PI_2, mb);
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
