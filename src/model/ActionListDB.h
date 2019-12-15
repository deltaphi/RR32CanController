#ifndef __MODEL__ACTIONLISTDB_H__
#define __MODEL__ACTIONLISTDB_H__

#include <list>
#include "model/TurnoutTypes.h"

namespace model {

namespace ActionListDB {

using Index_t = uint16_t;
using ActionList_t = std::list<model::TurnoutAction>;
using DB_t = std::list<ActionList_t>;

bool load(DB_t& db);
void store(const DB_t& db);

}  // namespace ActionListDB

}  // namespace model

#endif  // __MODEL__ACTIONLISTDB_H__
