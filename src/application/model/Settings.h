#ifndef __APPLICATION__MODEL__SETTINGS_H__
#define __APPLICATION__MODEL__SETTINGS_H__

namespace application {
namespace model {

/*
 * \brief Class Settings
 */
class Settings {
 public:
  enum class CommunicationChannel_t { CAN, WIFI };

  // char wifiSSID[64] = "";
  // char wifiKey[64] = "";

  CommunicationChannel_t communicationChannel = CommunicationChannel_t::CAN;

 private:
};

}  // namespace model
}  // namespace application

#endif  // __APPLICATION__MODEL__SETTINGS_H__
