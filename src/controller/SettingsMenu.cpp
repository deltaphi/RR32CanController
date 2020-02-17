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
  settings.store();
  masterControl.enterLocoControl();
}

void SettingsMenu::advanceMenu(MenuItemIndex_t menuItem,
                               MasterControl& masterControl) {
  switch (menuItem) {
    case 0:
      settings.store();
      masterControl.enterTurnoutMenu();
      break;
#if (CAN_ENABLED == STD_ON)
    case 1:
      // disable WiFi, enable CAN
      settings.data.communicationChannel =
          model::Settings::CommunicationChannel_t::CAN;
      break;
#endif
    case 2:
      // disable CAN, enable WiFi
      settings.data.communicationChannel =
          model::Settings::CommunicationChannel_t::WIFI;
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
