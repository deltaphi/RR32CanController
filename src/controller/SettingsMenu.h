#ifndef __CONTROLLER__SETTINGSMENU_H__
#define __CONTROLLER__SETTINGSMENU_H__

#include "controller/AbstractMenu.h"
#include "model/InputState.h"

namespace controller {

class MasterControl;

/*
 * \brief Class SettingsMenu
 */
class SettingsMenu : public AbstractMenu {
 public:
  static constexpr const uint8_t kNumMenuEntries = 1;

 protected:
  /// Callback when a menu item is selected.
  void advanceMenu(MenuItemIndex_t menuItem,
                   MasterControl& masterControl) override;

  /// Callback when the menu is aborted (shift+encoder)
  void abortMenu(MasterControl& masterControl) override;

  /**
   * \brief Callback to retrive an array of menuitems.
   *
   * This is called fairly frequently, thus it should be implemented
   * efficiently.
   */
  MenuItems_t getMenuItems() override;

  /**
   * \brief Callback to obtian the current known length of the menu.
   *
   * The length is not assumed to be constant for a given menu.
   */
  MenuItemIndex_t getTotalMenuLength() override;

 private:
  static const char* kTurnoutMapping;
  static const char* kActionListMapping;
  static const char* kWifiSettingsMapping;
  static const char* kConnectionSettingsMapping;

  static const char* kMenuEntries[kNumMenuEntries];
};

}  // namespace controller

#endif  // __CONTROLLER__SETTINGSMENU_H__
