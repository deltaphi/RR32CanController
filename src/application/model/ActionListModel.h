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


  uint8_t getNumActionLists() const { return db.size(); }

  std::size_t size() const { return db.size(); }

  DB_t& getDb() { return db; }

  void printActionList(Index_t index) const;
  void printActionLists(const char* serializedPrefix) const;

 private:
  DB_t db;

};

}  // namespace model
}  // namespace application

#endif  // __APPLICATION__MODEL__ACTIONLISTMODEL_H__
