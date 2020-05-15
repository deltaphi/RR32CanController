#ifndef __HAL__STORAGE__TURNOUTMAP_H__
#define __HAL__STORAGE__TURNOUTMAP_H__

#include "application/controller/TurnoutMapStorageCbk.h"

namespace hal {
namespace storage {

/*
 * \brief Class TurnoutMap
 */
class TurnoutMap : public application::controller::TurnoutMapStorageCbk {
 public:
  bool load(application::model::TurnoutMap& turnoutMap) override;
  void store(const application::model::TurnoutMap& turnoutMap) override;

 private:
  static const char* kTurnoutMapSettingsFile;
};

}  // namespace storage
}  // namespace hal

#endif  // __HAL__STORAGE__TURNOUTMAP_H__
