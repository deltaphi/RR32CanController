#ifndef __APPLICATION__CONTROLLER__ACTIONLISTSTORAGECBK_H__
#define __APPLICATION__CONTROLLER__ACTIONLISTSTORAGECBK_H__

#include "application/model/ActionListModel.h"

namespace application {
namespace controller {

/*
 * \brief Class ActionlistStorageCbk
 */    
class ActionlistStorageCbk {
  public:

  virtual bool load(application::model::ActionListModel::DB_t& db) = 0;
  virtual void store(const application::model::ActionListModel::DB_t& db) = 0;
};

}  // namespace controller
}  // namespace application

#endif  // __APPLICATION__CONTROLLER__ACTIONLISTSTORAGECBK_H__
