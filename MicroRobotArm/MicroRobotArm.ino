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
    doingControlledMovements,
    error
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

void drillingUpElbowUpAndDown(MoveBatch mb)
{
    mb = robotArmIK.runIK(50.0, 190.0, M_PI, mb);
    mb.addMove(/*id:*/ 3, /*pos:*/ 0);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(50.0, 170.0, M_PI, mb);
    mb.addMove(/*id:*/ 3, /*pos:*/ 1024);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(50.0, 190.0, M_PI, mb);
    mb.addMove(/*id:*/ 3, /*pos:*/ 0);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(50.0, 190.0, M_PI, mb, false);
    mb.addMove(/*id:*/ 3, /*pos:*/ 0);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(50.0, 170.0, M_PI, mb, false);
    mb.addMove(/*id:*/ 3, /*pos:*/ 1024);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(50.0, 190.0, M_PI, mb, false);
    mb.addMove(/*id:*/ 3, /*pos:*/ 0);
    sineStepperController.addMoveBatch(mb);
}

void lookingUpAndDown(MoveBatch mb)
{
    mb = robotArmIK.runIK(134.0, 60.0, M_PI_2 + 0.4, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(144.0, 94.0, M_PI_2, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(144.0, 94.0, M_PI_2 - 0.4, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(144.0, 94.0, M_PI_2 + 0.4, mb);
    sineStepperController.addMoveBatch(mb);
}

void drillingLeft(MoveBatch mb)
{
    mb = robotArmIK.runIK(124.0, 74.0, M_PI_2, mb);
    mb.addMove(/*id:*/ 3, /*pos:*/ 0);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(144.0, 74.0, M_PI_2, mb);
    mb.moveDuration = 2.5;
    mb.addMove(/*id:*/ 3, /*pos:*/ 2048);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(124.0, 74.0, M_PI_2, mb);
    mb.moveDuration = 2.5;
    mb.addMove(/*id:*/ 3, /*pos:*/ 0);
    sineStepperController.addMoveBatch(mb);

    mb.moveDuration = 1.7;
}

void slidingAlongFloor(MoveBatch mb)
{
    mb = robotArmIK.runIK(94.0, 10.0, 0.0, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(74.0, 10.0, 0.0, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(94.0, 10.0, 0.0, mb);
    sineStepperController.addMoveBatch(mb);
}

void repeatabilityTest(MoveBatch mb)
{
    // get into ready position.
    mb = robotArmIK.runIK(120.0, 90.0, M_PI_2, mb);
    sineStepperController.addMoveBatch(mb);

    // down -> up -> down.
    mb = robotArmIK.runIK(140.0, 60.0, M_PI_2, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(140.0, 90.0, M_PI_2, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(140.0, 120.0, M_PI_2, mb);
    sineStepperController.addMoveBatch(mb);

    // stop for measurement
    mb = robotArmIK.runIK(140.0, 120.0, M_PI_2, mb);
    mb.moveDuration = 0.5;
    sineStepperController.addMoveBatch(mb);
    mb.moveDuration = 1.75;

    mb = robotArmIK.runIK(140.0, 90.0, M_PI_2, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(140.0, 60.0, M_PI_2, mb);
    sineStepperController.addMoveBatch(mb);

    // get into second ready position
    mb = robotArmIK.runIK(85.0, 40.0, 0.0, mb);
    sineStepperController.addMoveBatch(mb);

    // near -> far -> near
    mb = robotArmIK.runIK(75.0, 20.0, 0.0, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(95.0, 20.0, 0.0, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(115.0, 20.0, 0.0, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(95.0, 20.0, 0.0, mb);
    sineStepperController.addMoveBatch(mb);

    mb = robotArmIK.runIK(75.0, 20.0, 0.0, mb);
    sineStepperController.addMoveBatch(mb);

    // stop for measurement
    mb = robotArmIK.runIK(75.0, 20.0, 0.0, mb);
    mb.moveDuration = 0.5;
    sineStepperController.addMoveBatch(mb);
    mb.moveDuration = 1.75;

    // get into second ready position
    mb = robotArmIK.runIK(85.0, 40.0, 0.0, mb);
    sineStepperController.addMoveBatch(mb);
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
    mb.moveDuration = 1.75;

    repeatabilityTest(mb);
    //drillingUpElbowUpAndDown(mb);
    //lookingUpAndDown(mb);
    //drillingLeft(mb);
    //slidingAlongFloor(mb);

    // go back to gome position START
    //mb.moveDuration = 1.7;
    //mb.addMove(/*id:*/ 0, /*pos:*/ 0);
    //mb.addMove(/*id:*/ 1, /*pos:*/ 0);
    //mb.addMove(/*id:*/ 2, /*pos:*/ 0);
    //mb.addMove(/*id:*/ 3, /*pos:*/ 0);
    //sineStepperController.addMoveBatch(mb);
    //sineStepperController.addMoveBatch(mb);
    // go back to gome position END

    if (robotArmIK.nanErrorOccured)
    {
        currentMode = error;
    }

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
