#include <avr/sleep.h>
#include <avr/wdt.h>
#include <Wire.h>
#include <I2C_RTC.h>

// clock
static DS1307 RTC;
// led
#define BLUE 5
#define GREEN 6
#define RED 7
// buttons
#define CHECK 2
#define SKIP 3
// timing definitions
#define DAY_SECONDS 86400
#define CONTACT_DAYS 14
#define CONTACT_DAYS_IN_SECONDS DAY_SECONDS* CONTACT_DAYS
#define SLEEP_SECONDS 8

volatile unsigned long elapsed_seconds = 0;
volatile unsigned long last_recorded_seconds;
volatile bool check_pressed = false;
volatile bool skip_pressed = false;
uint8_t rgb_values[15][3];

// Watchdog ISR routine
ISR(WDT_vect) {
  wdt_disable();  // disable watchdog
                  // Return from ISR
}

void checkInterrupt() {
  check_pressed = true;
}

void skipInterrupt() {
  skip_pressed = true;
}

void powerDown(int seconds) {
  // disable ADC
  ADCSRA = 0;

  // clear all MCUSR flags
  MCUSR = 0;

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  noInterrupts();  // timed sequence follows
  sleep_enable();

  // turn off brown-out detector
  MCUCR = bit(BODS) | bit(BODSE);
  MCUCR = bit(BODS);
  interrupts();
  sleep_cpu();

  // disable sleep
  sleep_disable();
}


void setup() {
  RTC.begin();

  pinMode(CHECK, INPUT_PULLUP);
  pinMode(SKIP, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CHECK), checkInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(SKIP), skipInterrupt, FALLING);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  // Populate the first 7 values with purple to lime
  for (int i = 0; i < 7; i++) {
    uint8_t intensity = (i + 1) * 255 / 7;  // 0 to 255

    rgb_values[i][0] = 255 - intensity * 155 / 255;  // Red
    rgb_values[i][1] = intensity * 200 / 255;        // Green
    rgb_values[i][2] = 255 - intensity * 155 / 255;  // Blue
  }

  // Populate the next 7 values with green to red
  for (int i = 7; i < 14; i++) {
    uint8_t intensity = (i - 6) * 255 / 7;           // Scale green intensity from 0 to 255
    rgb_values[i][0] = intensity;                    // Red
    rgb_values[i][1] = 155 - intensity * 155 / 255;  // Green
    rgb_values[i][2] = 0;                            // Blue scaled into 100 - 255 as 100 is where it makes an affect
  }
}

void displayRemainingTime() {
  volatile uint8_t current_day = elapsed_seconds / DAY_SECONDS;

  if (current_day > 14) {
    current_day = 14;
  }

  analogWrite(RED, rgb_values[current_day][0]);
  analogWrite(GREEN, rgb_values[current_day][1]);
  analogWrite(BLUE, rgb_values[current_day][2]);
  delay(1000);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);
}

void loop() {
  powerDown(SLEEP_SECONDS);
  // update time
  RTC.getEpoch(); // need to do a fake call otherwise time is messed up for some reason
  const long current_time = RTC.getEpoch();
  if (last_recorded_seconds != 0) {
    elapsed_seconds += current_time - last_recorded_seconds;
  }
  last_recorded_seconds = current_time;

  if (skip_pressed) {
    elapsed_seconds += DAY_SECONDS;  // Increment by one day in seconds
    // go back to 0 if over contact days. will reset timer.
    if (elapsed_seconds >= CONTACT_DAYS_IN_SECONDS) {
      elapsed_seconds = 0;
    }
    skip_pressed = false;
  }
  if (check_pressed) {
    displayRemainingTime();
    check_pressed = false;
  }
}
