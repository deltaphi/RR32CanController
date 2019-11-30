#include "controller/SettingsMenu.h"

#include "controller/MasterControl.h"

namespace controller {

const char* SettingsMenu::kTurnoutMapping = "Turnout mapping";
const char* SettingsMenu::kUseCAN = "Use CAN";
const char* SettingsMenu::kUseWifi = "Use WiFi (Hardcoded)";

const char* SettingsMenu::kActionListMapping = "Action list mapping";
const char* SettingsMenu::kWifiSettingsMapping = "WiFi Settings";
const char* SettingsMenu::kConnectionSettingsMapping = "Connection Settings";

const char* SettingsMenu::kMenuEntries[kNumMenuEntries] = {kTurnoutMapping,
                                                           kUseCAN, kUseWifi};

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

void SettingsMenu::getMenuItems(MenuItems_t& menuItems) {
  if (menuItems.offset >= kNumMenuEntries) {
    menuItems.numItems = 0;
    return;
  } else {
    int itemsToCopy = kNumMenuEntries - menuItems.offset;

    if (menuItems.numItems < itemsToCopy) {
      itemsToCopy = menuItems.numItems;
    }

    for (int i = 0; i < itemsToCopy; ++i) {
      menuItems.items[i] = kMenuEntries[i + menuItems.offset];
    }

    menuItems.numItems = itemsToCopy;
  }
}

SettingsMenu::MenuItemIndex_t SettingsMenu::getTotalMenuLength() {
  return kNumMenuEntries;
}

}  // namespace controller
