#include "RR32Can/messages/Data.h"

namespace RR32Can {

void Data::printAsHex() const {
  for (uint8_t i = 0; i < this->dlc; ++i) {
    printf(" %#02x", this->data[i]);
  }
}

void Data::printAsText() const {
  for (uint8_t i = 0; i < this->dlc; ++i) {
    printf("%c", static_cast<char>(this->data[i]));
  }
}

} /* namespace RR32Can */
