#ifndef __FORWARDER_H__
#define __FORWARDER_H__

#include "RR32Can/messages/Data.h"
#include "RR32Can/messages/Identifier.h"

void ForwardPacketToWifi(const RR32Can::Identifier& id,
                         const RR32Can::Data& data);
void ForwardPacketToCAN(const RR32Can::Identifier& id,
                        const RR32Can::Data& data);

#endif  // __FORWARDER_H__
