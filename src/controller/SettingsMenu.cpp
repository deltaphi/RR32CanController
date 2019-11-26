#include "controller/SettingsMenu.h"

#include "controller/MasterControl.h"

namespace controller {

const char* SettingsMenu::kTurnoutMapping = "Turnout mapping";
const char* SettingsMenu::kActionListMapping = "Action list mapping";
const char* SettingsMenu::kWifiSettingsMapping = "WiFi Settings";
const char* SettingsMenu::kConnectionSettingsMapping = "Connection Settings";

const char* SettingsMenu::kMenuEntries[kNumMenuEntries] = {kTurnoutMapping};

void SettingsMenu::begin() {
  settings.begin();
  AbstractMenu::begin();
}

void SettingsMenu::abortMenu(MasterControl& masterControl) {
  masterControl.enterLocoControl();
}

void SettingsMenu::advanceMenu(MenuItemIndex_t menuItem,
                               MasterControl& masterControl) {
  switch (menuItem) {
    case 0:
      masterControl.enterTurnoutMenu();
      break;
    default:
      printf("SettingsMenu: Unknown menu item %i.\n", menuItem);
      break;
  }
}

AbstractMenu::MenuItems_t SettingsMenu::getMenuItems() {
  MenuItems_t menuItems;
  menuItems.items = kMenuEntries;
  menuItems.offset = 0;
  menuItems.numItems = kNumMenuEntries;
  return menuItems;
}

SettingsMenu::MenuItemIndex_t SettingsMenu::getTotalMenuLength() {
  return kNumMenuEntries;
}

}  // namespace controller
