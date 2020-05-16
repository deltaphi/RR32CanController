#include "Arduino.h"

#include <chrono>
#include <thread>

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

unsigned long micros() {
  std::chrono::system_clock::time_point currently =
      std::chrono::system_clock::now();

  std::chrono::duration dura = currently.time_since_epoch();

  std::chrono::microseconds mic =
      std::chrono::duration_cast<std::chrono::microseconds>(dura);

  return mic.count();
}

unsigned long millis() {
  std::chrono::system_clock::time_point currently =
      std::chrono::system_clock::now();

  std::chrono::duration dura = currently.time_since_epoch();

  std::chrono::milliseconds mil =
      std::chrono::duration_cast<std::chrono::milliseconds>(dura);

  return mil.count();
}

void delay(uint32_t ms) {
  std::chrono::milliseconds chrono_ms(ms);
  std::this_thread::sleep_for(chrono_ms);
}