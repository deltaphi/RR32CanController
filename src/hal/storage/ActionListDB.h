#ifndef __HAL__STORAGE__ACTIONLISTDB_H__
#define __HAL__STORAGE__ACTIONLISTDB_H__

#include "application/controller/ActionlistStorageCbk.h"

namespace hal {
namespace storage {

/*
 * \brief Class ActionListDB
 */
class ActionListDB : public application::controller::ActionlistStorageCbk {
 public:
  bool load(application::model::ActionListModel::DB_t& db) override;
  void store(const application::model::ActionListModel::DB_t& db) override;
};

}  // namespace storage
}  // namespace hal

#endif  // __HAL__STORAGE__ACTIONLISTDB_H__
