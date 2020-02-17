#ifndef __CANMANAGER_H__
#define __CANMANAGER_H__

#include "config.h"

#if (CAN_ENABLED == STD_ON)

#include "RR32Can/messages/Data.h"
#include "RR32Can/messages/Identifier.h"

/*
 * \brief Class canManager
 */
class canManager {
 public:
  /// Initialize this class.
  void begin();

  void loop();

  /// Enable the CAN controller.
  void startCan();

  /// Disable the CAN controller.
  void stopCan();

  /// Whether the CAN controller is active and the bus is active.
  bool isActive() const;

  void SendPacket(const RR32Can::Identifier& id, const RR32Can::Data& data);

 private:
  /// Whether CAN should be used, independently of whether it is active
  bool canEnabled = false;

  /// Whether the CAN is not in an error state
  bool canBusActive = false;
};

#endif // CAN_ENABLED

#endif  // __CANMANAGER_H__
