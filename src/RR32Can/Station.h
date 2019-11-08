#ifndef __RR32CAN__STATION_H__
#define __RR32CAN__STATION_H__

#include <cstdint>

#include "RR32Can/Engine.h"
#include "RR32Can/EngineBrowser.h"
#include "RR32Can/EngineControl.h"
#include "RR32Can/Types.h"
#include "RR32Can/messages/Data.h"
#include "RR32Can/messages/Identifier.h"
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

  /**
   * \brief handle an incoming packet
   */
  void HandlePacket(const RR32Can::Identifier& id, const RR32Can::Data& data);

  /**
   * \brief Handle an incoming accessory command
   */
  void HandleAccessoryPacket(const RR32Can::Data& data);

  void HandleConfigDataStream(const RR32Can::Data& data);

  void HandleSystemCommand(const RR32Can::Data& data);

  /**
   * \brief Construct a Accessory command packet and send via CAN
   */
  void SendAccessoryPacket(uint32_t turnoutAddress, TurnoutDirection direction,
                           uint8_t power);

  /* Engine Database */
  void FinishCurrentConfigRequest();
  void AbortCurrentConfigRequest();

  /**
   * \brief Sends a request for the information of a specific engine.
   *
   * All incoming results will be directed towards the given engine (i.e., a
   * reference is kept).
   *
   * \param engine An engine with its name set.
   */
  void RequestEngine(Engine& engine);
  void RequestEngineList(uint8_t offset);
  void RequestEngineDirection(Engine& engine);
  void SendEngineDirection(Engine& engine, EngineDirection direction);
  void RequestEngineVelocity(Engine& engine);
  void SendEngineVelocity(Engine& engine, Engine::Velocity_t velocity);
  void SendEmergencyStop(Engine& engine);

  void SendSystemStop();
  void SendSystemGo();

  void HandleLocoDirection(const RR32Can::Data& data);
  void HandleLocoSpeed(const RR32Can::Data& data);

  EngineBrowser& getEngineBrowser() { return engineBrowser; }
  EngineControl& getEngineControl() { return engineControl; }

  void notifyConfigStreamReceived() { FinishCurrentConfigRequest(); }

  ConfigDataStreamParser::StreamState getConfigStreamState() const {
    return configDataParser.getStreamState();
  }

 private:
  Engine* getLocoForData(const RR32Can::Data& data);

  /* Initialization & Infrastructure */
  uint16_t senderHash;

  /* Generic message handling */
  ConfigDataStreamType expectedConfigData;
  ConfigDataStreamParser configDataParser;

  /* Engine Database */
  Engine engineDatabase[kMaxNumEnginesKnown];

  /* Engine browser */
  EngineBrowser engineBrowser;

  /* Single-Engine Controller */
  EngineControl engineControl;
};

}  // namespace RR32Can

#endif  // __RR32CAN__STATION_H__
