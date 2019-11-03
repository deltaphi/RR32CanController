#ifndef __RR32Can__HANDLER_H__
#define __RR32Can__HANDLER_H__

#include "RR32Can/Types.h"
#include "RR32Can/messages/Data.h"
#include "RR32Can/messages/Identifier.h"

namespace RR32Can {

/**
 * \brief Send an arbitrary packet via CAN
 */
void SendPacket(const RR32Can::Identifier& id, const RR32Can::Data& data);

} /* namespace RR32Can */

#endif