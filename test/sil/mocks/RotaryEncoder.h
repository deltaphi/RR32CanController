// -----
// RotaryEncoder.h - Library for using rotary encoders.
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See
// http://www.mathertel.de/License.aspx More information on:
// http://www.mathertel.de/Arduino
// -----
// 18.01.2014 created by Matthias Hertel
// 16.06.2019 pin initialization using INPUT_PULLUP
// -----

#ifndef RotaryEncoder_h
#define RotaryEncoder_h

#include "gmock/gmock.h"
#include "gtest/gtest.h"

class RotaryEncoder {
 public:
  enum class Direction { NOROTATION = 0, CLOCKWISE = 1, COUNTERCLOCKWISE = -1 };

  // ----- Constructor -----
  RotaryEncoder(int pin1, int pin2) {}

  // retrieve the current position
  MOCK_METHOD(long, getPosition, ());

  // simple retrieve of the direction the knob was rotated at. 0 = No rotation,
  // 1 = Clockwise, -1 = Counter Clockwise
  MOCK_METHOD(Direction, getDirection, ());

  // adjust the current position
  MOCK_METHOD(void, setPosition, (long));

  // call this function every some milliseconds or by using an interrupt for
  // handling state changes of the rotary encoder.
  MOCK_METHOD(void, tick, ());

  // Returns the time in milliseconds between the current observed

  MOCK_METHOD(unsigned long, getMillisBetweenRotations, (), (const));
};

#endif

// End