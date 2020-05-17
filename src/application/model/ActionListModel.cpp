#include "application/model/ActionListModel.h"

#include <cstdio>

namespace application {
namespace model {

void ActionListModel::printActionList(application::model::ActionListModel::Index_t index) const {
  application::model::ActionListModel::DB_t::const_iterator dbIt = db.begin();
  std::advance(dbIt, index);

  for (const application::model::TurnoutAction& action : *dbIt) {
    print(action);
  }
}

void ActionListModel::printActionLists(const char* serializedPrefix) const {
  printf("Printing %i Action Lists:\n", db.size());

  application::model::ActionListModel::DB_t::const_iterator dbIt = db.begin();
  int dbIdx = 0;
  while (dbIt != db.end()) {
    /* Human-readable */
    printf("Action List %i (%i elements):\n", dbIdx, dbIt->size());
    for (const application::model::TurnoutAction& action : *dbIt) {
      print(action);
    }

    /* Serialized form */
    printf("ActionList compact:\n");
    printf("%s %i", serializedPrefix,
           RR32Can::HumanTurnoutAddress(RR32Can::MachineTurnoutAddress(dbIdx)).value());

    for (const application::model::TurnoutAction& action : *dbIt) {
      printf(" %i %i", RR32Can::HumanTurnoutAddress(action.address).value(),
             static_cast<uint8_t>(action.direction));
    }

    printf("\n");

    ++dbIt;
    ++dbIdx;
  }

  printf("done.\n");
}

}  // namespace model
}  // namespace application
