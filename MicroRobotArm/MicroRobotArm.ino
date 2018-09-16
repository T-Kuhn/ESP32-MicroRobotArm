/*
ESP32 MicroRobotArm
 */

const double FREQ = 0.0005;
const double BASE_AMPLITUDE = 1000;

#define STEP 2
#define DIR 4
#define EXECUTING_ISR_CODE 5

hw_timer_t *timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint32_t isrCounter = 0;
int posX;
bool isMovingCW = true;

uint32_t pulseFromAmplitudeX(double ampl, double c)
{
  uint32_t s = (uint32_t)(c * ampl);
  uint32_t res = s % 2;

  if (res > digitalRead(STEP))
  {
    // count the positive flank.
    posX += isMovingCW ? 1 : -1;
  }
  return res;
}

void IRAM_ATTR onTimer()
{
  digitalWrite(EXECUTING_ISR_CODE, HIGH);

  portENTER_CRITICAL_ISR(&timerMux);
  isrCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);

  xSemaphoreGiveFromISR(timerSemaphore, NULL);

  // Sin Thing
  double c = cos(isrCounter * FREQ) + 1.0;

  uint32_t xStepBit = pulseFromAmplitudeX(BASE_AMPLITUDE, c);
  // Sin Thing
  digitalWrite(STEP, xStepBit);

  digitalWrite(EXECUTING_ISR_CODE, LOW);
}

void setup()
{
  Serial.begin(115200);

  // Set BTN_STOP_ALARM to input mode
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(EXECUTING_ISR_CODE, OUTPUT);

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
  timerAlarmWrite(timer, 512, true);

  // Start an alarm
  timerAlarmEnable(timer);
}

void loop()
{
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
    delay(1000);
  }
}

// MEMO:
//
// TODO:
//
// DOING:
// - implement that sin thing.
// - implement a mechanism to see how much time we spend in the ISR.
//
// DONE:
//
