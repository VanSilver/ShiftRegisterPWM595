/*
* If use ESP8266, ESP32 does not need to do
* - If you use Platformio, add the following line to Platformio.ini:
*   board_build.f_cpu = 160000000L
*
* - If you use ArduinoIDE, do the following:
*   Tools - CPU Frequency -> Select 160Mhz
*/

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

  // Begin with Medium speed (note: USE ESP8266 do not use SuperFast)
  sr.begin(sr.Medium);
}

void loop()
{
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 255; j += 25)
    {
      sr.set(i, j);
      delay(100);
    }
    sr.set(i, 255);
  }
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 255; j += 25)
    {
      sr.set(i, 255 - j);
      delay(100);
    }
    sr.set(i, 0);
  }
}

