#ifndef __MODEL__SETTINGS_H__
#define __MODEL__SETTINGS_H__

#include <cstddef>

namespace model {

/*
 * \brief Class Settings
 */
class Settings {
 public:
  enum class CommunicationChannel_t { CAN, WIFI };

  using Data = struct Data {
    // char wifiSSID[64] = "";
    // char wifiKey[64] = "";

    CommunicationChannel_t communicationChannel = CommunicationChannel_t::CAN;
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
