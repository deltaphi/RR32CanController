#ifndef __RR32Can__HANDLER_H__
#define __RR32Can__HANDLER_H__

#include "RR32Can/messages/Data.h"
#include "RR32Can/messages/Identifier.h"
#include "RR32Can/types.h"

namespace RR32Can {

/**
 * \brief handle an incoming packet
 */
void HandlePacket(const RR32Can::Identifier& id, const RR32Can::Data& data);

/**
 * \brief Handle an incoming accessory command
 */
void HandleAccessoryPacket(const RR32Can::Data& data);

/**
 * \brief Send an arbitrary packet via CAN
 */
void SendPacket(const RR32Can::Identifier& id, const RR32Can::Data& data);

/**
 * \brief Construct a Accessory command packet and send via CAN
 */
void SendAccessoryPacket(uint32_t turnoutAddress, TurnoutDirection direction,
                         uint8_t power);

} /* namespace RR32Can */

#endif