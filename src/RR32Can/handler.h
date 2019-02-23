#ifndef __RR32Can__HANDLER_H__
#define __RR32Can__HANDLER_H__

#include "RR32Can/Data.h"
#include "RR32Can/Identifier.h"

namespace RR32Can {

/**
 * \brief handle an incoming packet
 */
void HandlePacket(const RR32Can::Identifier& id,
                  const RR32Can::Data& data);

/**
 * \brief Handle an incoming accessory command
 */
void HandleAccessoryPacket(const RR32Can::Data& data);

/**
 * \brief Send an arbitrary packet via CAN
 */
void SendPacket(const RR32Can::Identifier& id,
                const RR32Can::Data& data);

/**
 * \brief Construct a Accessory command packet and send via CAN
 */
void SendAccessoryPacket(uint32_t turnoutAddress, TurnoutDirection direction,
                         uint8_t power);

/**
 * \brief Construct a data query packet with the given payload and send via CAN.
 *
 * \param data The payload data to be sent. Exactly charCount bytes are sent.
 * \param charCount the number of bytes to send. Must not be greater than 8.
 * Additional bytes are silently discarded.
 */
void SendRequestConfigDataPacket(const char* data, uint8_t charCount);

} /* namespace RR32Can */

#endif