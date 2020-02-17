#ifndef __MODEL__SETTINGS_H__
#define __MODEL__SETTINGS_H__

#include <cstddef>

#include "config.h"

namespace model {

/*
 * \brief Class Settings
 */
class Settings {
 public:
  enum class CommunicationChannel_t { 
#if (CAN_ENABLED == STD_ON)
    CAN = 0, 
#endif
    WIFI = 1 };

  using Data = struct Data {
    // char wifiSSID[64] = "";
    // char wifiKey[64] = "";
#if (CAN_ENABLED == STD_ON)
    CommunicationChannel_t communicationChannel = CommunicationChannel_t::CAN;
#else
    CommunicationChannel_t communicationChannel = CommunicationChannel_t::WIFI;
#endif
  };

  void begin();

  void store();

  Data data;

 private:
  bool load();

  static size_t loadData(Data* data);

  static const char* kSettingsFilename;
};

}  // namespace model

#endif  // __MODEL__SETTINGS_H__
