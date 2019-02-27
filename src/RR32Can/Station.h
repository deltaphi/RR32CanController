#ifndef __RR32CAN__STATION_H__
#define __RR32CAN__STATION_H__

#include <cstdint>

#include "RR32Can/Data.h"
#include "RR32Can/Engine.h"
#include "RR32Can/types.h"

namespace RR32Can {

/*
 * \brief Class Station
 */
class Station {
 public:
  /* Initialization & Infrastructure */
  void begin(uint16_t stationUUID);

  /* Generic message handling */
  void HandleConfigDataStream(const RR32Can::Data& data);

  /* Engine Database */
  void RequestEngine(Engine& engine);
  void RequestEngineList(uint8_t offset);

 private:
  /* Initialization & Infrastructure */
  uint16_t senderHash;

  /* Generic message handling */
  ConfigDataStreamType expectedConfigData;

  /* Engine Database */
  uint8_t numEnginesKnownByMaster;
  Engine engineDatabase[kMaxNumEnginesKnown];
};

}  // namespace RR32Can

#endif  // __RR32CAN__STATION_H__
