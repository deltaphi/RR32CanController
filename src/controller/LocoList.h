#ifndef __CONTROLLER__LOCOLIST_H__
#define __CONTROLLER__LOCOLIST_H__

#include <RR32Can/LocoListConsumer.h>
#include <RR32Can/Locomotive.h>
#include "controller/AbstractMenu.h"
#include "application/model/InputState.h"
#include "application/model/DisplayModel.h"

namespace controller {

class MasterControl;

/*
 * \brief Class LocoList
 */
class LocoList : public AbstractMenu {
 public:
  void begin();
  void loop(application::model::InputState& inputState,
            MasterControl& masterControl) override;

  /// Callback when a menu item is selected.
  void advanceMenu(MenuItemIndex_t menuItem,
                   MasterControl& masterControl) override;

  /// Callback when the menu is aborted (shift+encoder)
  void abortMenu(MasterControl& masterControl) override;

  const RR32Can::LocomotiveShortInfo* getSelectedEngine();

  void getMenuItems(MenuItems_t& menuItems) override;

  void notifyEncoderMoved(MenuItemIndex_t newItem) override;

  MenuItemIndex_t getTotalMenuLength() override {
    return browser.getNumEnginesKnownByMaster();
  }

  void RequestDownloadAtCursor();

 private:
  RR32Can::LocoListConsumer browser;
};

}  // namespace controller

#endif  // __CONTROLLER__LOCOLIST_H__
