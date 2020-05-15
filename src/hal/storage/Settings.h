#ifndef __HAL__STORAGE__SETTINGS_H__
#define __HAL__STORAGE__SETTINGS_H__

#include <cstddef>

#include "application/model/Settings.h"
#include "application/controller/SettingsStorageCbk.h"


namespace hal {
namespace storage {

/*
 * \brief Class Settings
 */    
class Settings: public application::controller::SettingsStorageCbk {
 public:

  void store(const application::model::Settings* const data) override;

  bool load(application::model::Settings* data) override;
  size_t loadData(application::model::Settings* data) override;

 private:
  static const char* kSettingsFilename;
};

}  // namespace storage
}  // namespace hal

#endif  // __HAL__STORAGE__SETTINGS_H__
