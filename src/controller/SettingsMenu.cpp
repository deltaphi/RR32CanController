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
      printf("SettingsMenu: Unknown menu item %li.\n", menuItem);
      break;
  }
}

AbstractMenu::MenuItems_t SettingsMenu::getMenuItems() {
  MenuItems_t menuItems;
  // TODO: This loop needs to be adjusted when there are more menu entries that
  // fit in .items.
  for (int i = 0; i < kNumMenuEntries; ++i) {
    menuItems.items[i] = kMenuEntries[i];
  }
  menuItems.offset = 0;
  menuItems.numItems = kNumMenuEntries;
  return menuItems;
}

SettingsMenu::MenuItemIndex_t SettingsMenu::getTotalMenuLength() {
  return kNumMenuEntries;
}

}  // namespace controller
