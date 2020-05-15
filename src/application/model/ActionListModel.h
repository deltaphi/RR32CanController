#ifndef __APPLICATION__MODEL__ACTIONLISTMODEL_H__
#define __APPLICATION__MODEL__ACTIONLISTMODEL_H__

#include <list>
#include "application/model/TurnoutTypes.h"

namespace application {
namespace model {

/*
 * \brief Class ActionListModel
 */
class ActionListModel {
 public:
  using Index_t = uint16_t;
  using ActionList_t = std::list<application::model::TurnoutAction>;
  using DB_t = std::list<ActionList_t>;

 private:
};

}  // namespace model
}  // namespace application

#endif  // __APPLICATION__MODEL__ACTIONLISTMODEL_H__
