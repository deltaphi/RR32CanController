#ifndef SRC_RR32CAN_MESSAGES_IDENTIFIER_H_
#define SRC_RR32CAN_MESSAGES_IDENTIFIER_H_

#include <cstdint>

#include "RR32Can/util/utils.h"

namespace RR32Can {

/**
 * \brief Reflects the deserialized form of a CAN identifier as used by Marklin
 */
class Identifier {
 public:
  /// Priority. 4 bit value. Set to 0 for all practical purposes.
  uint8_t prio = 0;  // 4 bit

  /// The Marklin command
  uint8_t command = 0;

  /// Whether this is a response packet
  bool response = false;  // 1 bit

  /// The hash of the packet
  uint16_t hash = 0;

  Identifier() = default;
  Identifier(uint8_t command, uint16_t hash) : command(command), hash(hash) {}

  /**
   * \brief Turn a 32-bit CAN identifier into a Marklin Identifier
   */
  static Identifier GetIdentifier(unsigned long packetId);

  /**
   * \brief Turn a 4-byte buffer from LAN into a Marklin Identifier
   */
  static Identifier GetIdentifier(uint8_t buffer[4]);

  /**
   * \brief Determine the correct hash value given the address of this component
   */
  void computeAndSetHash(uint32_t deviceUuid) {
    hash = computeSenderHash(deviceUuid);
  }

  /**
   * \brief convert this Marklin Identifier into a 29bit CAN extended
   * identifier.
   */
  unsigned long makeIdentifier() const;

  /**
   * \brief Print the identifier to serial in human-readable form.
   */
  void printAll() const;
};

} /* namespace RR32Can */

#endif  // SRC_RR32CAN_MESSAGES_IDENTIFIER_H_
