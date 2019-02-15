#include "MaerklinCan/Data.h"

namespace MaerklinCan {

void Data::printAsHex() const {
  for (uint8_t i = 0; i < this->dlc; ++i) {
    Serial.print(" ");
    Serial.print(this->data[i], HEX);
  }
}

void Data::printAsText() const {
  for (uint8_t i = 0; i < this->dlc; ++i) {
    Serial.print(static_cast<char>(this->data[i]));
  }
}

} /* namespace MaerklinCan */
