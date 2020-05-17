#include "application/controller/LocoList.h"
#include <application/controller/MasterControl.h>

#include "RR32Can/RR32Can.h"
#include "application/view/UIAssets.h"

namespace application {
namespace controller {

void LocoList::begin() {
  browser.reset();
  limiter.setMin(0);
  limiter.setMax(DISPLAY_LINES + 1);
}

void LocoList::abortMenu(MasterControl& masterControl) {
  // switch to settings menu on shift+encoder
  masterControl.enterSettingsMenu();
}

void LocoList::advanceMenu(MenuItemIndex_t menuItem, MasterControl& masterControl) {
  // switch to download on encoder. This will commit the current engine.
  masterControl.enterLocoDownload();
}

void LocoList::loop(application::model::InputState& inputState, MasterControl& masterControl) {
  application::controller::AbstractMenu::loop(inputState, masterControl);
  if (RR32Can::RR32Can.getConfigStreamState() ==
      RR32Can::ConfigDataStreamParser::StreamState::STREAM_DONE) {
    forceDisplayUpdate();
  }
}

void LocoList::notifyEncoderMoved(MenuItemIndex_t newItem) {
  // Determine whether additional items need downloading.

  if (newItem < browser.getStreamOffset() ||
      newItem >= browser.getStreamOffset() + RR32Can::kEngineBrowserEntries) {
    browser.clearTable();
    RequestDownloadAtCursor();
  }
}

void LocoList::getMenuItems(MenuItems_t& menuItems) {
  switch (RR32Can::RR32Can.getConfigStreamState()) {
    case RR32Can::ConfigDataStreamParser::StreamState::IDLE:
    case RR32Can::ConfigDataStreamParser::StreamState::WAITING_FIRST_PACKET:
    case RR32Can::ConfigDataStreamParser::StreamState::WAITING_DATA_PACKET: {
      // No data available, print "pending" dots.
      // TODO: Limit to the actual number of locos pending, i.e., compare with
      // getNumEnginesKnownByMaster()
      for (int i = 0; i < menuItems.numItems; ++i) {
        menuItems.items[i] = application::view::kDots;
      }
    } break;
    case RR32Can::ConfigDataStreamParser::StreamState::STREAM_DONE: {
      // Engine list is present. Update display.
      // Copy interesting entries to display

      const RR32Can::LocoListConsumer::EngineInfoSet& locoInfo = browser.getEngineInfos();

      int numItems =
          (RR32Can::kNumEngineNamesDownload < menuItems.numItems ? RR32Can::kNumEngineNamesDownload
                                                                 : menuItems.numItems);

      for (int i = 0; i < numItems; ++i) {
        menuItems.items[i] = locoInfo[i].getName();
      }

      menuItems.numItems = numItems;

      limiter.setMax(browser.getNumEnginesKnownByMaster() - 1);

      // Resets the STREAM_DONE flag.
      RR32Can::RR32Can.notifyConfigStreamReceived();

    } break;
  }
}

void LocoList::RequestDownloadAtCursor() {
  // Integer division to find the correct element to request for download
  MenuItemIndex_t cursorPosition = getCurrentItem();
  MenuItemIndex_t offset = cursorPosition - (cursorPosition % RR32Can::kNumEngineNamesDownload);
  browser.setStreamOffset(offset);
  browser.clearTable();
  RR32Can::RR32Can.RequestEngineList(offset, browser);
}

const RR32Can::LocomotiveShortInfo* LocoList::getSelectedEngine() {
  const RR32Can::LocoListConsumer::EngineInfoSet& engineInfos = browser.getEngineInfos();
  uint8_t index = getCurrentItem() - browser.getStreamOffset();
  if (index > engineInfos.size()) {
    return nullptr;
  } else if (!engineInfos[index].isNameKnown()) {
    return nullptr;
  } else {
    return &engineInfos[index];
  }
}

}  // namespace controller
}  // namespace application
