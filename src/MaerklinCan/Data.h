#ifndef __MAERKLINCAN__DATA_H__
#define __MAERKLINCAN__DATA_H__

#include <Arduino.h>

#include "MaerklinCan/constants.h"

namespace MaerklinCan {

/**
 * \brief Class to represent the payload of a Marklin CAN packet.
 *
 * This class represents the untyped payload and can be sent or received from
 * CAN.
 */
struct Data {
  /// Data Length C... . actual calue ist 4 bit only.
  uint8_t dlc = 0;
  uint8_t data[CanDataMaxLength] = {0, 0, 0, 0, 0, 0, 0};

  /**
   * \brief Print the DLC and Data field as Hex values.
   */
  void printAsHex() const;

  /**
   * \brief Print the Payload as characters.
   */
  void printAsText() const;
};

} /* namespace MaerklinCan */

#endif