#ifndef __CONTROLLER__LOCOLIST_H__
#define __CONTROLLER__LOCOLIST_H__

#include <RR32Can/LocoListConsumer.h>
#include <RR32Can/Locomotive.h>
#include "model/InputState.h"
#include "view/DisplayManager.h"

namespace controller {

class MasterControl;

/*
 * \brief Class LocoList
 */
class LocoList {
 public:
  using CursorPosition_t = int16_t;

  void begin();
  void loop(model::InputState& inputState, MasterControl& masterControl);

  const RR32Can::LocomotiveShortInfo* getSelectedEngine();

  void updateDisplay(view::DisplayManager& displayManager);

  CursorPosition_t getCursorPosition() const { return cursorPosition; }

  void RequestDownloadAtCursor();

 private:
  CursorPosition_t cursorPosition;
  RR32Can::LocoListConsumer browser;
};

}  // namespace controller

#endif  // __CONTROLLER__LOCOLIST_H__
