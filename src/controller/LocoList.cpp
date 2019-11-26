#include "controller/LocoList.h"
#include <controller/MasterControl.h>

#include "RR32Can/RR32Can.h"
#include "view/UIAssets.h"

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

void LocoList::advanceMenu(MenuItemIndex_t menuItem,
                           MasterControl& masterControl) {
  // switch to download on encoder. This will commit the current engine.
  masterControl.enterLocoDownload();
}

void LocoList::loop(model::InputState& inputState,
                    MasterControl& masterControl) {
  AbstractMenu::loop(inputState, masterControl);
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

AbstractMenu::MenuItems_t LocoList::getMenuItems() {
  MenuItems_t result;
  result.numItems = RR32Can::kNumEngineNamesDownload;
  result.offset = getMenuItemInFirstDisplayLine();

  switch (RR32Can::RR32Can.getConfigStreamState()) {
    case RR32Can::ConfigDataStreamParser::StreamState::IDLE:
    case RR32Can::ConfigDataStreamParser::StreamState::WAITING_FIRST_PACKET:
    case RR32Can::ConfigDataStreamParser::StreamState::WAITING_DATA_PACKET: {
      // No data available, print "pending" dots.
      for (int i = 0; i < RR32Can::kNumEngineNamesDownload; ++i) {
        result.items[i] = view::kDots;
      }
    } break;
    case RR32Can::ConfigDataStreamParser::StreamState::STREAM_DONE: {
      // Engine list is present. Update display.
      // Copy interesting entries to display

      uint8_t line = 0;
      for (const RR32Can::LocomotiveShortInfo& info :
           browser.getEngineInfos()) {
        result.items[line] = info.getName();
        ++line;
      }

      limiter.setMax(browser.getNumEnginesKnownByMaster() - 1);

      // Resets the STREAM_DONE flag.
      RR32Can::RR32Can.notifyConfigStreamReceived();

    } break;
  }

  return result;
}

void LocoList::RequestDownloadAtCursor() {
  // Integer division to find the correct element to request for download
  MenuItemIndex_t cursorPosition = getCurrentItem();
  MenuItemIndex_t offset =
      cursorPosition - (cursorPosition % RR32Can::kNumEngineNamesDownload);
  browser.setStreamOffset(offset);
  browser.clearTable();
  RR32Can::RR32Can.RequestEngineList(offset, browser);
}

const RR32Can::LocomotiveShortInfo* LocoList::getSelectedEngine() {
  const RR32Can::LocoListConsumer::EngineInfoSet& engineInfos =
      browser.getEngineInfos();
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
