#ifndef __CONTROLLER__LOCOLIST_H__
#define __CONTROLLER__LOCOLIST_H__

#include "RR32Can/Engine.h"

#include "RR32Can/EngineBrowser.h"
#include "model/InputState.h"
#include "view/DisplayManager.h"

namespace controller {

class UIControl;

/*
 * \brief Class LocoList
 */
class LocoList {
 public:
  using CursorPosition_t = int16_t;

  void begin();
  void loop(model::InputState& inputState, UIControl& uiControl);

  const RR32Can::EngineShortInfo* getSelectedEngine();

  void updateDisplay(view::DisplayManager& displayManager);

  CursorPosition_t getCursorPosition() const { return cursorPosition; }

  void RequestDownloadAtCursor();

 private:
  CursorPosition_t cursorPosition;
  RR32Can::EngineBrowser browser;
};

}  // namespace controller

#endif  // __CONTROLLER__LOCOLIST_H__
