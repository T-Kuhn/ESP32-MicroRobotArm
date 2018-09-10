/*
 Repeat timer example

 This example shows how to use hardware timer in ESP32. The timer calls onTimer
 function every second. The timer can be stopped with button attached to PIN 0
 (IO0).

 This example code is in the public domain.
 */

#define STEP 2
#define DIR 4

// TestPins
#define TP1 16
#define TP2 17
#define TP3 18
#define TP4 19

#define EXECUTING_ISR_CODE 5

hw_timer_t *timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint32_t isrCounter = 0;

void IRAM_ATTR onTimer()
{
  digitalWrite(EXECUTING_ISR_CODE, HIGH);

  digitalWrite(TP1, HIGH);
  digitalWrite(TP2, HIGH);
  digitalWrite(TP3, HIGH);

  digitalWrite(TP1, LOW);
  digitalWrite(TP2, LOW);
  digitalWrite(TP3, LOW);

  // Increment the counter of the ISR
  portENTER_CRITICAL_ISR(&timerMux);
  isrCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);

  if (cos((double)isrCounter * 0.01) > 0.4)
  {
    digitalWrite(TP4, LOW);
  }
  else
  {
    digitalWrite(TP4, HIGH);
  }

  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(timerSemaphore, NULL);

  // It is safe to use digitalRead/Write here if you want to toggle an output
  int currentStepLevel = digitalRead(STEP);
  digitalWrite(STEP, !currentStepLevel);

  digitalWrite(EXECUTING_ISR_CODE, LOW);
}

void setup()
{
  Serial.begin(115200);

  // Set BTN_STOP_ALARM to input mode
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(EXECUTING_ISR_CODE, OUTPUT);

  pinMode(TP1, OUTPUT);
  pinMode(TP2, OUTPUT);
  pinMode(TP3, OUTPUT);
  pinMode(TP4, OUTPUT);

  digitalWrite(DIR, HIGH);

  // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();

  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
  // info).
  timer = timerBegin(0, 80, true);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);

  // Set alarm to call onTimer function every x microseconds.
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, 700, true);

  // Start an alarm
  timerAlarmEnable(timer);
}

void loop()
{
  delay(1000);

  // If Timer has fired
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE)
  {
    uint32_t isrCount = 0;

    // Read the interrupt count
    portENTER_CRITICAL(&timerMux);
    isrCount = isrCounter;
    portEXIT_CRITICAL(&timerMux);

    // Print it
    //Serial.print("cnt: ");
    //Serial.println(isrCount);
  }
}

// MEMO:
//
// TODO:
// - implement that sin thing.
//
// DOING:
// - implement a mechanism to see how much time we spend in the ISR.
//
// DONE:
//
