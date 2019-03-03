#ifndef __RR32CAN__STATION_H__
#define __RR32CAN__STATION_H__

#include <cstdint>

#include "RR32Can/Engine.h"
#include "RR32Can/EngineBrowser.h"
#include "RR32Can/messages/Data.h"
#include "RR32Can/types.h"
#include "RR32Can/util/ConfigDataConsumer.h"
#include "RR32Can/util/ConfigDataStreamParser.h"

namespace RR32Can {

/*
 * \brief Class Station
 */
class Station {
 public:
  /* Initialization & Infrastructure */
  void begin(uint16_t stationUUID);
  void loop();

  /* Generic message handling */
  void HandleConfigDataStream(const RR32Can::Data& data);

  /* Engine Database */
  void AbortCurrentConfigRequest();
  void RequestEngine(Engine& engine);
  void RequestEngineList(uint8_t offset);

  EngineBrowser & getEngineBrowser() { return engineBrowser; }

  void notifyConfigStreamReceived() {
    configDataParser.reset();
  };

  ConfigDataStreamParser::StreamState getConfigStreamState() const {
    return configDataParser.getStreamState();
  }

 private:
  /* Initialization & Infrastructure */
  uint16_t senderHash;

  /* Generic message handling */
  ConfigDataStreamType expectedConfigData;
  ConfigDataStreamParser configDataParser;
  ConfigDataConsumer* activeConfigDataConsumer = nullptr;

  /* Engine Database */
  Engine engineDatabase[kMaxNumEnginesKnown];

  /* Engine browser */
  EngineBrowser engineBrowser;
};

}  // namespace RR32Can

#endif  // __RR32CAN__STATION_H__
