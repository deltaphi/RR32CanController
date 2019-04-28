#include "RR32Can/Handler.h"

#include <CAN.h>

#include "config.h"

#include "RR32Can/RR32Can.h"

#include "RR32Can/messages/Data.h"
#include "RR32Can/messages/TurnoutPacket.h"

#include "RR32Can/util/BufferManager.h"
#include "RR32Can/util/TextParser.h"

namespace RR32Can {

void SendPacket(const RR32Can::Identifier& id, const RR32Can::Data& data) {
  // Send packet on CAN
  CAN.beginExtendedPacket(id.makeIdentifier());
  for (int i = 0; i < data.dlc; ++i) {
    CAN.write(data.data[i]);
  }
  CAN.endPacket();
}

} /* namespace RR32Can */
