#include "application/controller/AbstractMenu.h"

#include <cstdio>
#include <cstring>

namespace application {
namespace controller {

void AbstractMenu::begin() {
  currentItem = 0;
  limiter.setMax(getTotalMenuLength());
};

void AbstractMenu::loop(application::model::InputState& inputState, MasterControl& masterControl) {
  if (inputState.isEncoderRisingEdge()) {
    if (inputState.isShiftPressed()) {
      printf("AbstractMenu: Abort Menu.\n");
      abortMenu(masterControl);
    } else {
      printf("AbstractMenu: Advance on item %li.\n", currentItem);
      advanceMenu(currentItem, masterControl);
    }
  } else {
    // Evaluate moving through the list

    if (inputState.isEncoderMoved()) {
      // Encoder moved
      currentItem = inputState.encoder.getPosition();
      MenuItemIndex_t limitedValue = limiter.limitedValue(currentItem);
      if (limitedValue == currentItem) {
        // performed within limits.
        inputState.consumeEncoderPosition();
        notifyEncoderMoved(limitedValue);
        displayUpdateNeeded = true;

      } else {
        // Moved out of configured bounds, limit the encoder.
        inputState.loadEncoderPosition(limitedValue);
      }
    }
  }
}

void AbstractMenu::forceDisplayUpdate() {
  displayUpdateNeeded = true;
  // Set to a value that is guaranteed to trigger an update of the menu.
  menuItemInFirstDisplayLine = currentItem + 1;
}

void AbstractMenu::updateDisplay(application::model::DisplayModel& displayManager) {
  if (displayUpdateNeeded) {
    if (currentItem < menuItemInFirstDisplayLine ||
        currentItem >= menuItemInFirstDisplayLine + kCountMenuItems) {
      // Move the menu *up* or *down*

      // Compute new menu item to be in the first line
      menuItemInFirstDisplayLine = (currentItem / kCountMenuItems) * kCountMenuItems;
      // printf("Move menu. New Top: %i. ", menuItemInFirstDisplayLine);

      // Fetch the required strings
      MenuItems_t menuItems;
      menuItems.offset = menuItemInFirstDisplayLine;
      menuItems.numItems = DISPLAY_LINES;
      getMenuItems(menuItems);

      // Check if the requested elements are in range
      if (menuItems.offset <= menuItemInFirstDisplayLine &&
          menuItemInFirstDisplayLine <= menuItems.offset + menuItems.numItems) {
        // The start element is contained.
        // We assume that currentItem was checked in the loop.

        // Copy items. Start at menuItemInFirstDisplayLine - offset and go for
        // min(numStrings, DISPLAY_LINES);
        MenuItemIndex_t itemsToCopy = DISPLAY_LINES;
        if (menuItems.numItems < itemsToCopy) {
          itemsToCopy = menuItems.numItems;
        }
        // printf("Copying %i out of %i elements. ", itemsToCopy,
        // menuItems.numItems);

        for (int i = 0; i < itemsToCopy; ++i) {
          int stringIndex = (menuItemInFirstDisplayLine - menuItems.offset + i);
          // printf("From %i to %i. ", stringIndex, i);
          displayManager.updateBuffer(menuItems.items[stringIndex], STRING_CHAR_LENGTH, i);
        }

        // Set remaining lines to be empty
        for (int i = itemsToCopy; i < DISPLAY_LINES; ++i) {
          strncpy(displayManager.getWritableBuffer(i), "", 1);
        }
      }
    }

    // Also move the cursor
    displayManager.enableCursor();
    // printf("Move cursor to %i.", currentItem - menuItemInFirstDisplayLine);
    displayManager.setCursorLine(currentItem - menuItemInFirstDisplayLine);

    displayUpdateNeeded = false;
    // printf("\n");
  }
}

void AbstractMenu::notifyEncoderMoved(MenuItemIndex_t newItem) {
  // Dummy implementation that is guaranteed to do nothing.
}

}  // namespace controller
}  // namespace application
