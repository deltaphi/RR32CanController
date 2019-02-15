#include "MaerklinCan/Identifier.h"

namespace MaerklinCan {

Identifier Identifier::GetIdentifier(unsigned long packetId) {
  Identifier id;

  id.hash = static_cast<uint16_t>(packetId & 0xFFFF);
  packetId >>= 16;  // Shift away hash

  id.response = (packetId & 0x01) != 0;
  packetId >>= 1;  // Shift away response bit

  id.command = static_cast<uint8_t>(packetId & 0xFF);
  packetId >>= 8;  // Shift away command bit

  id.prio = static_cast<uint8_t>(packetId & 0x0F);

  return id;
}

unsigned long Identifier::makeIdentifier() const {
  unsigned long packetId;

  packetId = prio << (16 + 1 + 8);
  packetId |= command << (16 + 1);
  packetId |= response << (16);
  packetId |= hash;

  return packetId;
}

void Identifier::computeAndSetHash(uint32_t uid) {
  uid = ((uid >> 3) & 0xFFFFFF00) | (uid & 0x7F);
  uint16_t lowBytes = uid;
  uint16_t highBytes = (uid >> 16);
  hash = lowBytes ^ highBytes;
}

void Identifier::printAll() const {
  Serial.print(F("Prio: 0x"));
  Serial.print(this->prio, HEX);

  Serial.print(F(" Command: 0x"));
  Serial.print(this->command, HEX);

  Serial.print(F(" Response: 0x"));
  Serial.print(this->response, HEX);

  Serial.print(F(" Hash: 0x"));
  Serial.print(this->hash, HEX);
}

} /* namespace MaerklinCan */
