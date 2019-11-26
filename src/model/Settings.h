#ifndef __MODEL__SETTINGS_H__
#define __MODEL__SETTINGS_H__

namespace model {

/*
 * \brief Class Settings
 */
class Settings {
 public:
  void begin();

  void store();

 private:
  bool load();

  static const char* kSettingsFilename;

  using Data = struct Data {
    char wifiSSID[64] = "";
    char wifiKey[64] = "";

    bool useWifi = false;
    bool useCAN = true;
  };

  Data data;
};

}  // namespace model

#endif  // __MODEL__SETTINGS_H__
