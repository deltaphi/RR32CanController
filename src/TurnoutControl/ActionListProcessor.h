#ifndef __ACTION_LIST_PROCESSOR_H__
#define __ACTION_LIST_PROCESSOR_H__

#include "RR32Can/Types.h"

#include "model/ActionListDB.h"

namespace TurnoutControl {

class ActionListProcessor {
 public:
  void begin() {
    model::ActionListDB::load(db);
    setInactive();
  }

  typedef struct {
    uint8_t address;
    RR32Can::TurnoutDirection direction;
  } Action;

  bool hasActiveAction() const { return currentActionList != db.end(); }

  bool requestActionList(uint8_t actionListIndex);

  void loop();

  uint8_t getNumActionLists() const { return db.size(); }

  std::size_t size() const { return db.size(); }

  void printActionList(model::ActionListDB::Index_t index) const;
  void printActionLists(const char* serializedPrefix) const;

  model::ActionListDB::DB_t& getDb() { return db; }

 private:
  model::ActionListDB::DB_t db;

  void performAction();

  void setInactive() {
    currentActionList = db.end();
    currentAction = model::ActionListDB::ActionList_t::iterator();
    buttonPressed = false;
  }

  /**
   * The currently active action list, if any.
   */
  model::ActionListDB::DB_t::iterator currentActionList;

  /**
   * The currently active action out of the above action list.
   * nullptr denotes that no action list is actove or that processing of the
   * active list has not started.
   */
  model::ActionListDB::ActionList_t::iterator currentAction;

  /**
   * Note whether the last action was a button press (true) or release (false).
   */
  bool buttonPressed = false;

  unsigned long lastActionTime;
  constexpr static unsigned long requestedSleepDuration =
      200000;  // Sleep for 200ms, i.e, 200'000us
};

} /* namespace TurnoutControl */

#endif
