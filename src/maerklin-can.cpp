#include "maerklin-can.h"

// Global instance of the system
MaerklinSystemClass MaerklinSystem;

MaerklinCanIdentifier MaerklinCanIdentifier::GetIdentifier(unsigned long packetId) {
	MaerklinCanIdentifier id;

	id.hash = static_cast<uint16_t>(packetId & 0xFFFF);
	packetId >>= 16; // Shift away hash
  
  id.response = (packetId & 0x01) != 0;
  packetId >>= 1; // Shift away response bit
  
  id.command = static_cast<uint8_t>(packetId & 0xFF);
  packetId >>= 8; // Shift away command bit
  
  id.prio = static_cast<uint8_t>(packetId & 0x0F);

	return id;
}

unsigned long MaerklinCanIdentifier::makeIdentifier() const {
  unsigned long packetId;

  packetId = prio << (16 + 1 + 8);
  packetId |= command << (16 + 1);
  packetId |= response << (16);
  packetId |= hash;

  return packetId;
}

void MaerklinCanIdentifier::computeAndSetHash(uint32_t uid) {
  uid = ((uid >> 3) & 0xFFFFFF00) | (uid & 0x7F);
  uint16_t lowBytes = uid;
  uint16_t highBytes = (uid >> 16);
  hash = lowBytes ^ highBytes;
}

void MaerklinCanIdentifier::printAll() const {
  Serial.print(F("Prio: 0x"));
  Serial.print(this->prio, HEX);
  
  Serial.print(F(" Command: 0x"));
  Serial.print(this->command, HEX);
  
  Serial.print(F(" Response: 0x"));
  Serial.print(this->response, HEX);
  
  Serial.print(F(" Hash: 0x"));
  Serial.print(this->hash, HEX);
}

void HandleMaerklinCommand(const MaerklinCanIdentifier & id, const MaerklinCanData & data) {
  id.printAll();
  Serial.println();

  Serial.print(F("Command: "));
  switch (id.command) {
    case kSystemCommand:
      Serial.print(F("System Command. Subcommand: "));
      switch (data.data[4]) {
        case kSubcommandSystemGo:
        Serial.print(F("GO!"));
        MaerklinSystem.systemOn = true;
        break;
        case kSubcommandSystemHalt:
        Serial.print(F("Halt!"));
        break;
        case kSubcommandSystemStop:
        Serial.print(F("STOP!"));
        MaerklinSystem.systemOn = false;
        break;
        case kSubcommandSystemIdentifier:
        Serial.print(F("Identifier"));
        break;
        case kSubcommandSystemOverload:
        Serial.print(F("OVERLOAD!"));
        break;
        case kSubcommandSystemReset:
        Serial.print(F("Reset"));
        MaerklinSystem.systemOn = false;
        break;
        case kSubcommandSystemStatus:
        Serial.print(F("Status"));
        break;
        default:
        Serial.print(F("unknown"));
        break;
      }
      Serial.println();
      break;

    case kPing:
      Serial.print(F("Ping. Payload: 0x"));
      data.printAsHex();
      break;

    case kAccessorySwitch: 
      Serial.print(F("Accessory Switch. Details: "));
      processAccessory(data);
      break;

    case kRequestConfigData:
      Serial.print(F("Request Config Data. Payload: "));
      data.printAsText();
      break;

    case kConfigDataStream:
      Serial.print(F("Config Data Stream. Payload: "));
      data.printAsText();
      break;

    default:
      Serial.print(F("unknown or not implemented. Dump: 0x"));
      Serial.print(id.command, HEX);
      data.printAsHex();
      break;
  }
  Serial.println();
}

MaerklinTurnoutPacket MaerklinTurnoutPacket::FromCanPacket(const MaerklinCanData & data) {
  MaerklinTurnoutPacket turnoutPacket;
  
  turnoutPacket.locid = data.data[3] | (data.data[2] << 8) | (data.data[1] << 16) | (data.data[0] << 24); 
  turnoutPacket.position = data.data[4];
  turnoutPacket.power = data.data[5];

  return turnoutPacket;
}

void MaerklinTurnoutPacket::serialize(MaerklinCanData & data) const {
  data.dlc = 6;
  data.data[0] = (locid >> 24) & 0xFF;
  data.data[1] = (locid >> 16) & 0xFF;
  data.data[2] = (locid >> 8) & 0xFF;
  data.data[3] = locid & 0xFF;
  data.data[4] = position;
  data.data[5] = power;
}

uint32_t MaerklinTurnoutPacket::PositionAsHumanValue() const {
  decltype(locid) locid_human = (locid & 0x0FFF) + 1;
  return locid_human;
}

void processAccessory(const MaerklinCanData & data) {
  MaerklinTurnoutPacket turnoutPacket = MaerklinTurnoutPacket::FromCanPacket(data);
  turnoutPacket.printAll();
}

void MaerklinTurnoutPacket::printAll() const {
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

void MaerklinCanData::printAsHex() const {
  for (uint8_t i = 0; i < this->dlc; ++i) {
    Serial.print(" ");
    Serial.print(this->data[i], HEX);
  }
}

void MaerklinCanData::printAsText() const {
  for (uint8_t i = 0; i < this->dlc; ++i) {
    Serial.print(static_cast<char>(this->data[i]));
  }
}