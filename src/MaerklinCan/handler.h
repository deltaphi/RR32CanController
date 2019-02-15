#ifndef __MAERKLINCAN__HANDLER_H__
#define __MAERKLINCAN__HANDLER_H__

#include "MaerklinCan/Identifier.h"
#include "MaerklinCan/Data.h"

namespace MaerklinCan {

/**
 * \brief handle an incoming packet
 */
void HandlePacket(const MaerklinCan::Identifier& id,
                           const MaerklinCan::Data& data);

/**
 * \brief Handle an incoming accessory command
 */
void HandleAccessoryPacket(const MaerklinCan::Data& data);

/**
 * \brief Send an arbitrary packet via CAN
 */
void SendPacket(const MaerklinCan::Identifier& id,
                           const MaerklinCan::Data& data);

/**
 * \brief Construct a Accessory command packet and send via CAN
 */
void SendAccessoryPacket(uint32_t turnoutAddress, TurnoutDirection direction,
                       uint8_t power);

/**
 * \brief Construct a data query packet with the given payload and send via CAN.
 * 
 * \param data The payload data to be sent. Exactly charCount bytes are sent.
 * \param charCount the number of bytes to send. Must not be greater than 8. Additional bytes are silently discarded.
 */
void SendRequestConfigDataPacket(const char* data, uint8_t charCount);

} /* namespace MaerklinCan */

#endif