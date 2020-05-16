#ifndef __APPLICATION__CONTROLLER__SETTINGSSTORAGECBK_H__
#define __APPLICATION__CONTROLLER__SETTINGSSTORAGECBK_H__

#include "application/model/Settings.h"

namespace application {
namespace controller {

/*
 * \brief Class SettingsStorageCbk
 */
class SettingsStorageCbk {
 public:
  virtual void store(const application::model::Settings* const data) = 0;

  virtual bool load(application::model::Settings* data) = 0;
  virtual size_t loadData(application::model::Settings* data) = 0;

 private:
  static const char* kSettingsFilename;
};

}  // namespace controller
}  // namespace application

#endif  // __APPLICATION__CONTROLLER__SETTINGSSTORAGECBK_H__
