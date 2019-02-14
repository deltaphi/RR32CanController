#ifndef __MAERKLIN_CAN_H__
#define __MAERKLIN_CAN_H__

#include <Arduino.h>

// these are the command values as defined by Marklin
// CAN Identifier values are shifted left by one bit.

constexpr uint8_t kSystemCommand = 0x00;
constexpr uint8_t klocoDiscovery = 0x01;
constexpr uint8_t kMfxBind = 0x02;
constexpr uint8_t kMfxVerify = 0x03;
constexpr uint8_t kLocoSpeed = 0x04;
constexpr uint8_t kLocoDirection = 0x05;
constexpr uint8_t kLocoFunction = 0x06;

constexpr uint8_t kReadConfig = 0x07;
constexpr uint8_t kWriteConfig = 0x08;
constexpr uint8_t kAccessorySwitch = 0x0B;
constexpr uint8_t kAccessoryConfig = 0x0C;

constexpr uint8_t kS88Poll = 0x10;
constexpr uint8_t kS88Event = 0x11;
constexpr uint8_t kSX1Event = 0x12;

constexpr uint8_t kPing = 0x18;

constexpr uint8_t kUpdateOffer = 0x19;
constexpr uint8_t kReadConfigData = 0x1A;
constexpr uint8_t kBootloaderCan = 0x1B;
constexpr uint8_t kBootloaderTrack = 0x1C;
constexpr uint8_t kStatusdataConfig = 0x1D;
constexpr uint8_t kRequestConfigData = 0x20;
constexpr uint8_t kConfigDataStream = 0x21;
constexpr uint8_t k60128DataStream = 0x22;

// Subcommands for system commands

constexpr uint8_t kSubcommandSystemStop = 0x00;
constexpr uint8_t kSubcommandSystemGo = 0x01;
constexpr uint8_t kSubcommandSystemHalt = 0x02;
constexpr uint8_t kSubcommandLocoEmergencyStop = 0x03;
constexpr uint8_t kSubcommandLocoStopCycle = 0x04;
constexpr uint8_t kSubcommandLocoDataprotocol = 0x05;
constexpr uint8_t kSubcommandAccessoryTime = 0x06;
constexpr uint8_t kSubcommandMfxFastRead = 0x07;
constexpr uint8_t kSubcommandDataprotocolOn = 0x08;
constexpr uint8_t kSubcommandMfxSetRegistrationCount = 0x09;
constexpr uint8_t kSubcommandSystemOverload = 0x0A;
constexpr uint8_t kSubcommandSystemStatus = 0x0B;
constexpr uint8_t kSubcommandSystemIdentifier = 0x0C;
constexpr uint8_t kSubcommandMfxSeek = 0x30;
constexpr uint8_t kSubcommandSystemReset = 0x80;

class MaerklinCanIdentifier {
 public:
  uint8_t prio = 0;  // 4 bit
  uint8_t command = 0;
  bool response = false;  // 1 bit
  uint16_t hash = 0;

  static MaerklinCanIdentifier GetIdentifier(unsigned long packetId);
  void computeAndSetHash(uint32_t deviceUuid);

  unsigned long makeIdentifier() const;

  void printAll() const;
};

constexpr const uint8_t CanDataMaxLength = 8;

struct MaerklinCanData {
  uint8_t dlc = 0;  // Data Length C... - 4 bit
  uint8_t data[CanDataMaxLength] = {0, 0, 0, 0, 0, 0, 0};

  void printAsHex() const;
  void printAsText() const;
};

void HandleMaerklinCommand(const MaerklinCanIdentifier& id,
                           const MaerklinCanData& data);
void processAccessory(const MaerklinCanData& data);

class MaerklinTurnoutPacket {
 public:
  uint32_t locid = 0;
  uint8_t position = 0;
  uint8_t power = 0;

  static MaerklinTurnoutPacket FromCanPacket(const MaerklinCanData& data);
  void serialize(MaerklinCanData& data) const;

  uint32_t PositionAsHumanValue() const;
  void printAll() const;
};

class MaerklinSystemClass {
 public:
  bool systemOn;
};

extern MaerklinSystemClass MaerklinSystem;

#endif /* __MAERKLIN_CAN_H__ */
