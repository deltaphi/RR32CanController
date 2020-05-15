#include "Arduino.h"

#include <chrono>

unsigned long micros() {
  std::chrono::system_clock::time_point currently =
      std::chrono::system_clock::now();

  std::chrono::duration dura = currently.time_since_epoch();

  std::chrono::microseconds mic =
      std::chrono::duration_cast<std::chrono::microseconds>(dura);

  return dura.count();
}
