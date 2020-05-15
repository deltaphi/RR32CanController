// Fake Arduino include to support unit tests

#ifndef __ARDUINO_H__
#define __ARDUINO_H__

#include <cstdint>

#define LOW 0x0
#define HIGH 0x1

// Map function copied verbatim from https://www.arduino.cc/reference/en/language/functions/math/map/
long map(long x, long in_min, long in_max, long out_min, long out_max);

unsigned long micros();
unsigned long millis();

void delay(uint32_t ms);

#endif  // !define __ARDUINO_H__
