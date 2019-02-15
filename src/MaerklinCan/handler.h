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


void SendPacket(const MaerklinCan::Identifier& id,
                           const MaerklinCan::Data& data);

void SendAccessoryPacket(uint32_t turnoutAddress, TurnoutDirection direction,
                       uint8_t power);
} /* namespace MaerklinCan */

#endif