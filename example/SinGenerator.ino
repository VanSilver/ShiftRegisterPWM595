#include <Arduino.h>
#include "ShiftRegisterPWM595.h"

// Define 75HC595 pin
#define DATA_PIN 27
#define CLOCK_PIN 25
#define LATCH_PIN 26

// Number of translation registers (each IC 74HC595 is a translation register with 8 outputs)
#define SHIFT_REGISTER_COUNT 1 // Here use 1 IC 74HC595

// Resolution of PWM (maximum 255)
#define PWM_RESOLUTION 255
 
// Instantiation 
ShiftRegisterPWM595 sr(SHIFT_REGISTER_COUNT, PWM_RESOLUTION, DATA_PIN, CLOCK_PIN, LATCH_PIN);

void setup()
{
  Serial.begin(115200);

  // Begin with SupperFast speed
  sr.begin(ShiftRegisterPWM595::SuperFast);
}

void loop()
{
  for (uint8_t i = 0; i < 8; i++) {
    uint8_t val = (uint8_t)(((float)
      sin(millis() / 150.0 + i / 8.0 * 2.0 * PI) + 1) * 128);
    sr.set(i, val);
  }
}

