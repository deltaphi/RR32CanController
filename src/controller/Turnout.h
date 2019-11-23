#ifndef __CONTROLLER__TURNOUT_H__
#define __CONTROLLER__TURNOUT_H__

#include "TurnoutControl/ActionListProcessor.h"
#include "TurnoutControl/TurnoutTypes.h"
#include "model/InputState.h"

namespace controller {

/*
 * \brief Class Turnout
 */
class Turnout {
 public:
  void begin();
  void loop(model::InputState& inputState);

 private:
  void handleButton(uint8_t buttonIndex, uint8_t buttonState);
  void handleMultiturnout(TurnoutControl::TurnoutLookupResult result,
                          RR32Can::TurnoutDirection requestedDirection);

  TurnoutControl::ActionListProcessor actionListProcessor;
};

}  // namespace controller

#endif  // __CONTROLLER__TURNOUT_H__
