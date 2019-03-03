#include "RR32Can/messages/TurnoutPacket.h"

namespace RR32Can {

TurnoutPacket TurnoutPacket::FromCanPacket(const Data& data) {
  TurnoutPacket turnoutPacket;

  turnoutPacket.locid = data.data[3] | (data.data[2] << 8) |
                        (data.data[1] << 16) | (data.data[0] << 24);
  turnoutPacket.position = data.data[4];
  turnoutPacket.power = data.data[5];

  return turnoutPacket;
}

void TurnoutPacket::serialize(Data& data) const {
  data.dlc = 6;
  data.data[0] = (locid >> 24) & 0xFF;
  data.data[1] = (locid >> 16) & 0xFF;
  data.data[2] = (locid >> 8) & 0xFF;
  data.data[3] = locid & 0xFF;
  data.data[4] = position;
  data.data[5] = power;
}

uint32_t TurnoutPacket::PositionAsHumanValue() const {
  decltype(locid) locid_human = (locid & 0x0FFF) + 1;
  return locid_human;
}

void TurnoutPacket::printAll() const {
  Serial.print(F("Turnout: 0x"));
  Serial.print(this->locid, HEX);

  Serial.print(F(" Human Value: "));
  Serial.print(this->PositionAsHumanValue(), DEC);

  Serial.print(F(" Position: "));
  Serial.print(this->position, DEC);
  switch (this->position) {
    case 0:
      Serial.print(F(" (off, round, red)"));
      break;
    case 1:
      Serial.print(F(" (on, straight, green)"));
      break;
    case 2:
      Serial.print(F(" (yellow, left)"));
      break;
    case 3:
      Serial.print(F(" (white)"));
      break;
  }

  Serial.print(F(" Power: "));
  Serial.print(this->power, DEC);
  switch (this->power) {
    case 0:
      Serial.print(F(" (off, button release)"));
      break;
    case 1:
      Serial.print(F(" (on, button press)"));
      break;
  }
}

} /* namespace RR32Can */
