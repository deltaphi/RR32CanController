#include "controller/LocoList.h"
#include <controller/MasterControl.h>

#include "RR32Can/RR32Can.h"
#include "view/UIAssets.h"

namespace controller {

void LocoList::begin() {
  cursorPosition = 0;
  browser.reset();
}

void LocoList::loop(model::InputState& inputState,
                    MasterControl& masterControl) {
  if (inputState.isEncoderRisingEdge()) {
    if (inputState.isShiftPressed()) {
      // switch to settings menu on shift+encoder
      masterControl.enterSettingsMenu();
    } else {
      // switch to download on encoder. This will commit the current engine.
      masterControl.enterLocoDownload();
    }
  } else {
    // Run the usual locoList loop

    // handle the encoder
    // SELECT_ENGINE. Move the cursor by one. If the cursor goes out of
    // bounds, request new data download.

    cursorPosition = inputState.encoder.getPosition();
    if (inputState.lastEncoderState.position != cursorPosition) {
      inputState.lastEncoderState.position = cursorPosition;
      inputState.lastEncoderState.direction = inputState.encoder.getDirection();

      if (cursorPosition < browser.getStreamOffset()) {
        // Try to go up in the list
        if (browser.getStreamOffset() == 0 || cursorPosition < 0) {
          // can't go further. reject the change.
          cursorPosition = 0;
          inputState.loadEncoderPosition(cursorPosition);
        } else {
          browser.clearTable();
          RequestDownloadAtCursor();
        }
      } else if (cursorPosition >=
                 browser.getStreamOffset() + RR32Can::kEngineBrowserEntries) {
        // Try to go down in the list

        if (cursorPosition >= browser.getNumEnginesKnownByMaster()) {
          cursorPosition = browser.getNumEnginesKnownByMaster() - 1;
          inputState.loadEncoderPosition(cursorPosition);
        } else {
          browser.clearTable();
          RequestDownloadAtCursor();
        }
      }
    }
  }
}

void LocoList::updateDisplay(view::DisplayManager& displayManager) {
  switch (RR32Can::RR32Can.getConfigStreamState()) {
    case RR32Can::ConfigDataStreamParser::StreamState::WAITING_FIRST_PACKET:
    case RR32Can::ConfigDataStreamParser::StreamState::WAITING_DATA_PACKET: {
      // No data available, print "pending" dots.
      displayManager.disableCursor();
      strncpy(displayManager.getWritableBuffer(0), view::kDots,
              STRING_CHAR_LENGTH);
      strncpy(displayManager.getWritableBuffer(1), view::kDots,
              STRING_CHAR_LENGTH);
    } break;
    case RR32Can::ConfigDataStreamParser::StreamState::STREAM_DONE: {
      // Engine list is present. Update display.
      // Copy interesting entries to display
      uint8_t line = 0;
      for (const RR32Can::LocomotiveShortInfo& info :
           browser.getEngineInfos()) {
        strncpy(displayManager.getWritableBuffer(line), info.getName(),
                STRING_DATATYPE_LENGTH);
        ++line;
      }
      // Resets the STREAM_DONE flag.
      RR32Can::RR32Can.notifyConfigStreamReceived();

      displayManager.enableCursor();
      displayManager.setCursorLine(cursorPosition - browser.getStreamOffset());
    } break;
    case RR32Can::ConfigDataStreamParser::StreamState::IDLE:
      // no change to display
      displayManager.setCursorLine(cursorPosition - browser.getStreamOffset());
      break;
  }
}

void LocoList::RequestDownloadAtCursor() {
  // Integer division to find the correct element to request for download
  CursorPosition_t offset =
      cursorPosition - (cursorPosition % RR32Can::kNumEngineNamesDownload);
  browser.setStreamOffset(offset);
  browser.clearTable();
  RR32Can::RR32Can.RequestEngineList(offset, browser);
}

const RR32Can::LocomotiveShortInfo* LocoList::getSelectedEngine() {
  const RR32Can::LocoListConsumer::EngineInfoSet& engineInfos =
      browser.getEngineInfos();
  uint8_t index = cursorPosition - browser.getStreamOffset();
  if (index > engineInfos.size()) {
    return nullptr;
  } else if (!engineInfos[index].isNameKnown()) {
    return nullptr;
  } else {
    return &engineInfos[index];
  }
}

}  // namespace controller
