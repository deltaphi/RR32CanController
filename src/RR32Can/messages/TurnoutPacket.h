#ifndef SRC_RR32CAN_MESSAGES_TURNOUTPACKET_H_
#define SRC_RR32CAN_MESSAGES_TURNOUTPACKET_H_

#include <Arduino.h>

#include "RR32Can/messages/Data.h"

namespace RR32Can {

/**
 * \brief Resembles a turnout command.
 */
class TurnoutPacket {
 public:
  /// Turnout-address (0-based)
  uint32_t locid = 0;

  /// Whether to set the turnout to straight or branching
  uint8_t position = 0;

  /// Whether to switch the actuator on or off
  uint8_t power = 0;

  /**
   * \brief Parse a CAN payload into a turnout packet.
   *
   * This does not check the type of the contents of Data.
   */
  static TurnoutPacket FromCanPacket(const Data& data);

  /**
   * \brief Serialize the fields of this TurnoutPacket into a CAN payload.
   */
  void serialize(Data& data) const;

  /**
   * \brief Obtain the human-readable turnout address (1-based)
   */
  uint32_t PositionAsHumanValue() const;

  /**
   * \brief Print a human-readable version of this Turnout Packet.
   */
  void printAll() const;
};

} /* namespace RR32Can */

#endif  // SRC_RR32CAN_MESSAGES_TURNOUTPACKET_H_
