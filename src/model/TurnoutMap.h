#ifndef __MODEL__TURNOUTMAP_H__
#define __MODEL__TURNOUTMAP_H__

#include <cstdint>

#include "config.h"
#include "model/TurnoutTypes.h"

namespace model {

#define TURNOUT_MAP_LENGTH ((TURNOUT_BUTTONS_COUNT) / 2)

/*
 * \brief Class TurnoutMap
 */
class TurnoutMap {
 public:
  /**
   * "Loads" the mapping by setting up a 1:1 default mapping.
   */
  void begin();

  /**
   * Convers a button index to a turnout number.
   * Uses bus numbering, not human-readable numbering.
   */
  model::TurnoutLookupResult lookupTurnout(uint8_t buttonIndex) const;

 private:
  /**
   * \brief Maps from button index to turnout number.
   *
   * Note that index 0 here corresponds to button TURNOUT_BUTTONS_OFFSET.
   * Array is half the length of the buttons, as there are always two buttons
   * for a turnout.
   */
  model::TurnoutLookupResult turnoutMap[TURNOUT_MAP_LENGTH];
};

}  // namespace model

#endif  // __MODEL__TURNOUTMAP_H__
