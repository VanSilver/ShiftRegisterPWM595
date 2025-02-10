/***********************************************************************************************************
 *   Designed by VanSilver
 *   Contact:
 *   https://www.youtube.com/@vansilver1711
 *   https://facebook.com/VanSilver.ytb
 ***********************************************************************************************************/
/***********************************************************************************************************
 *   Library for PWM control of the 74HC595 shift register.
 *   This library is a modified version of the ShiftRegisterPWM library
 *   (https://github.com/Simsso/ShiftRegister-PWM-Library)
 *   to support the ESP32 and ESP8266 on the Arduino Framework.
 ***********************************************************************************************************/
#ifndef ShiftRegisterPWM595_h
#define ShiftRegisterPWM595_h

#include <Arduino.h>
#ifdef ESP32
#include "driver/timer.h"
#endif

class ShiftRegisterPWM595
{
public:
    // The interrupt frequency is determined based on the divider (divider = 80 -> 1MHz)
    // With these values, the update frequency (Hz) = 1e6 / alarm_value.
    enum UpdateFrequency
    {
        VerySlow, // ≈ 6,4000 Hz update => alarm_value ~ 156 (1e6/156 ≈ 6410 Hz)
        Slow,     // ≈ 12,800 Hz update => alarm_value ~ 78  (1e6/78  ≈ 12821 Hz)
        Medium,   // ≈ 25,600 Hz update => alarm_value ~ 39  (1e6/39  ≈ 25641 Hz)
        Fast,     // ≈ 35,714 Hz update => alarm_value ~ 28  (1e6/28  ≈ 35714 Hz)
        SuperFast // ≈ 51,281 Hz update => alarm_value ~ 20  (1e6/20  ≈ 50000 Hz)
    };

    ShiftRegisterPWM595(uint8_t shiftRegisterCount, uint8_t resolution, uint8_t dataPin, uint8_t clockPin, uint8_t latchPin);
    void set(uint8_t pin, uint8_t value);
    void update();
    void begin();
    void begin(UpdateFrequency freq);
    static void IRAM_ATTR onTimer();

private:
    void shiftOut(uint8_t data);
    static ShiftRegisterPWM595 *instance;
    uint8_t shiftRegisterCount;
    uint8_t resolution;
    uint8_t *data; // Array data for PWM: data[t + shiftRegister * resolution]
    bool singleShiftRegister;
    uint8_t dataPin, clockPin, latchPin;
    volatile uint8_t time; // Internal time variable from 0 to (Resolution-1)
#ifdef ESP32
    hw_timer_t *timer = NULL;
#endif

};

#endif
