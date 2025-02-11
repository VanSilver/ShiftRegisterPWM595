#include "ShiftRegisterPWM595.h"

ShiftRegisterPWM595 *ShiftRegisterPWM595::instance = nullptr;

ShiftRegisterPWM595::ShiftRegisterPWM595(uint8_t shiftRegisterCount, uint8_t resolution, uint8_t dataPin, uint8_t clockPin, uint8_t latchPin)
    : shiftRegisterCount(shiftRegisterCount), resolution(resolution), dataPin(dataPin), clockPin(clockPin), latchPin(latchPin), time(0)
{
    // Init GPIO
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(latchPin, OUTPUT);
    digitalWrite(latchPin, LOW);

    // allocate memory for PWM data array, initialize the number 0
    data = (uint8_t *)malloc(resolution * shiftRegisterCount * sizeof(uint8_t));
    memset(data, 0, resolution * shiftRegisterCount);
    instance = this;
    singleShiftRegister = (shiftRegisterCount == 1);
}

void ShiftRegisterPWM595::set(uint8_t pin, uint8_t value)
{
    // Transfer value 0-255 to the range [0, Resolution]
    value = (uint8_t)(value / 255.0 * resolution + 0.5);
    uint8_t shiftRegister = pin / 8;
    // For each step, set the bit of the led channel based on the comparative value
    for (int t = 0; t < resolution; ++t)
    {
        // set (pin % 8)th bit to (value > t)
        data[t + shiftRegister * resolution] ^= (-(value > t) ^ data[t + shiftRegister * resolution]) & (1 << (pin % 8));
    }
}

void ShiftRegisterPWM595::update()
{
    digitalWrite(latchPin, LOW);
    for (int i = shiftRegisterCount - 1; i >= 0; i--)
    {
        shiftOut(data[time + i * resolution]);
    }
    digitalWrite(latchPin, HIGH);
    if (++time == resolution)
    {
        time = 0;
    }
}

void ShiftRegisterPWM595::shiftOut(uint8_t dataByte)
{
    // shiftout data in order from the bit MSB to LSB
    for (int i = 0; i < 8; i++)
    {
        digitalWrite(clockPin, LOW);
        digitalWrite(dataPin, (dataByte & (1 << (7 - i))) ? HIGH : LOW);
        digitalWrite(clockPin, HIGH);
    }
}
void ShiftRegisterPWM595::begin()
{
    uint64_t alarm_value = 39; // Default Medium
#ifdef ESP32
    // Use Timer 0, Divider = 80 => Base frequency = 80mhz/80 = 1mHz
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &ShiftRegisterPWM595::onTimer, true);
    timerAlarmWrite(timer, alarm_value, true);
    timerAlarmEnable(timer);
#endif
#ifdef ESP8266
    noInterrupts();                                       // Disable timer interrupt before config
    timer1_isr_init();                                    // Init ISR for Timer1
    timer1_attachInterrupt(ShiftRegisterPWM595::onTimer); // set Callback
    timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);          // Base frequency = 80mhz/1
    timer1_write(alarm_value * 80);                       // 1us = 80tick
    interrupts();                                         // Enable timer interrupt
#endif
}

void ShiftRegisterPWM595::begin(UpdateFrequency freq)
{
    uint64_t alarm_value = 39; // Default Medium
    switch (freq)
    {
    case VerySlow:
        alarm_value = 156;
        break;
    case Slow:
        alarm_value = 78;
        break;
    case Medium:
        alarm_value = 39;
        break;
    case Fast:
        alarm_value = 28;
        break;
    case SuperFast:
        alarm_value = 20;
        break;
    }
#ifdef ESP32
    // Use Timer 0, Divider = 80 => Base frequency = 80mhz/80 = 1mHz
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &ShiftRegisterPWM595::onTimer, true);
    timerAlarmWrite(timer, alarm_value, true);
    timerAlarmEnable(timer);
#endif
#ifdef ESP8266
    noInterrupts();                                       // Disable timer interrupt before config
    timer1_isr_init();                                    // Init ISR for Timer1
    timer1_attachInterrupt(ShiftRegisterPWM595::onTimer); // set Callback
    timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);          // Base frequency = 80mhz/1
    timer1_write(alarm_value * 80);                       // 1us = 80tick
    interrupts();                                         // Enable timer interrupt
#endif
}

void IRAM_ATTR ShiftRegisterPWM595::onTimer()
{
    // cli(); //disable interrupt
    if (instance)
    {
        instance->update();
    }
    // sei(); //enable interrupt
}