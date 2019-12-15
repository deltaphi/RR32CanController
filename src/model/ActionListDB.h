#ifndef __MODEL__ACTIONLISTDB_H__
#define __MODEL__ACTIONLISTDB_H__

#include <list>
#include "model/TurnoutTypes.h"

namespace model {

/*
 * \brief Class ActionListDB
 */
class ActionListDB {
 public:
  using Index_t = uint16_t;
  using ActionList_t = std::list<model::TurnoutAction>;
  using DB_t = std::list<ActionList_t>;

  void begin();

  void store();

  std::size_t size() const { return db.size(); }

  DB_t& getDb() { return db; }

  const DB_t& getDb() const { return db; }

 private:
  bool load();

  static const char* kActionListFilename;

  DB_t db;

  struct FileHeader_t {
    Index_t numLists;
  };

  struct ListHeader_t {
    Index_t listLength;
  };
};

}  // namespace model

#endif  // __MODEL__ACTIONLISTDB_H__
