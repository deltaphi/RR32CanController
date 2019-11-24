#ifndef __CONTROLLER__TURNOUT_H__
#define __CONTROLLER__TURNOUT_H__

#include "TurnoutControl/ActionListProcessor.h"
#include "model/InputState.h"
#include "model/TurnoutMap.h"
#include "model/TurnoutTypes.h"

namespace controller {

/*
 * \brief Class Turnout
 */
class Turnout {
 public:
  void begin();
  void loop(model::InputState& inputState);

  model::TurnoutMap& getTurnoutMap() { return turnoutMap; };

 private:
  void handleButton(uint8_t buttonIndex, uint8_t buttonState);
  void handleMultiturnout(model::TurnoutLookupResult result,
                          RR32Can::TurnoutDirection requestedDirection);

  TurnoutControl::ActionListProcessor actionListProcessor;

  model::TurnoutMap turnoutMap;
};

}  // namespace controller

#endif  // __CONTROLLER__TURNOUT_H__