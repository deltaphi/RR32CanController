#ifndef __RR32CAN__STATION_H__
#define __RR32CAN__STATION_H__

#include <cstdint>

#include "RR32Can/Engine.h"
#include "RR32Can/EngineBrowser.h"
#include "RR32Can/EngineControl.h"
#include "RR32Can/StationCbk.h"
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
  void begin(uint16_t stationUUID, StationCbk& callback);
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
  void RequestEngine(Engine& engine,
                     RR32Can::EngineControl& configDataConsumer);
  void RequestEngineList(uint8_t offset,
                         RR32Can::EngineBrowser& configDataConsumer);

  void RequestEngineDirection(Engine& engine);
  void SendEngineDirection(Engine& engine, EngineDirection direction);
  void RequestEngineVelocity(Engine& engine);
  void SendEngineVelocity(Engine& engine, Engine::Velocity_t velocity);
  void RequestEngineFunction(Engine& engine, uint8_t function);
  void RequestEngineAllFunctions(Engine& engine);
  void SendEngineFunction(Engine& engine, uint8_t function, bool value);
  void SendEmergencyStop(Engine& engine);

  void SendSystemStop();
  void SendSystemGo();

  void HandleLocoDirection(const RR32Can::Data& data);
  void HandleLocoSpeed(const RR32Can::Data& data);
  void HandleLocoFunction(const RR32Can::Data& data);

  void notifyConfigStreamReceived() { FinishCurrentConfigRequest(); }

  ConfigDataStreamParser::StreamState getConfigStreamState() const {
    return configDataParser.getStreamState();
  }

 private:
  Engine* getLocoForData(const RR32Can::Data& data);

  static Engine::Uid_t uidFromData(const uint8_t* ptr);
  static void uidToData(uint8_t* ptr, Engine::Uid_t uid);

  /* Initialization & Infrastructure */
  uint16_t senderHash;

  /* Set during begin() */
  StationCbk* callback = nullptr;

  /* Generic message handling */
  ConfigDataStreamType expectedConfigData;
  ConfigDataStreamParser configDataParser;
};

}  // namespace RR32Can

#endif  // __RR32CAN__STATION_H__
