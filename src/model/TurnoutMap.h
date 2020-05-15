#ifndef __MODEL__TURNOUTMAP_H__
#define __MODEL__TURNOUTMAP_H__

#include <cstdint>

#include "config.h"
#include "application/model/TurnoutTypes.h"

namespace model {

#define TURNOUT_MAP_LENGTH ((TURNOUT_BUTTONS_COUNT) / 2)

/*
 * \brief Class TurnoutMap
 */
class TurnoutMap {
 public:
  using ButtonIndex_t = uint8_t;

  /**
   * "Loads" the mapping by setting up a 1:1 default mapping.
   */
  void begin();

  void store();

  /**
   * Converts a button index to a turnout number.
   * Uses bus numbering, not human-readable numbering.
   */
  application::model::TurnoutLookupResult lookupTurnout(ButtonIndex_t buttonIndex) const;

  void setLookupTurnout(ButtonIndex_t buttonIndex,
                        application::model::TurnoutLookupResult newResult);

  void print() const;

 private:
  bool load();

  static const char* kTurnoutMapSettingsFile;

  ButtonIndex_t buttonToArrayIndex(ButtonIndex_t button) const {
    button /= 2;  // Ignore the lowest bit
    return button;
  }

  application::model::TurnoutLookupResult& rangeCheckedMapAt(ButtonIndex_t button);
  application::model::TurnoutLookupResult rangeCheckedMapAt(ButtonIndex_t button) const {
    return const_cast<TurnoutMap*>(this)->rangeCheckedMapAt(button);
  }

  using TurnoutMap_t = application::model::TurnoutLookupResult[TURNOUT_MAP_LENGTH];

  /**
   * \brief Maps from button index to turnout number.
   *
   * Note that index 0 here corresponds to button TURNOUT_BUTTONS_OFFSET.
   * Array is half the length of the buttons, as there are always two buttons
   * for a turnout.
   */
  TurnoutMap_t turnoutMap;
};

}  // namespace model

#endif  // __MODEL__TURNOUTMAP_H__
