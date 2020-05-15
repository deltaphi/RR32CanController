#ifndef __CONTROLLER__ABSTRACTMENU_H__
#define __CONTROLLER__ABSTRACTMENU_H__

#include <cstdint>

#include "controller/EncoderLimiter.h"
#include "model/InputState.h"
#include "application/model/DisplayModel.h"

namespace controller {

class MasterControl;

/*
 * \brief Base Class AbstractMenu
 *
 * Shows a menu of kCountMenuItems lines length.
 * Tracks which menu item is selected and displays a cursor accordingly.
 * Fetches new strings on demand, when the cursor moves out of the current
 * window.
 */
class AbstractMenu {
 public:
  using MenuItemIndex_t = long;
  static constexpr const uint8_t kCountMenuItems = DISPLAY_LINES;

  virtual void begin();
  virtual void loop(model::InputState& inputState,
                    MasterControl& masterControl);

  void updateDisplay(application::model::DisplayModel& displayManager);
  /**
   * \brief Force rereading the display lines on the next draw cycle.
   */
  void forceDisplayUpdate();

  struct MenuItems_t {
    const char* items[DISPLAY_LINES];
    MenuItemIndex_t offset;
    MenuItemIndex_t numItems;
  };

  MenuItemIndex_t getCurrentItem() const { return currentItem; }

 protected:
  /// Callback when a menu item is selected.
  virtual void advanceMenu(MenuItemIndex_t menuItem,
                           MasterControl& masterControl) = 0;

  /// Callback when the menu is aborted (shift+encoder)
  virtual void abortMenu(MasterControl& masterControl) = 0;

  /**
   * \brief Callback to retrive an array of menuitems.
   *
   * This is called fairly frequently, thus it should be implemented
   * efficiently.
   *
   * \param menuItems [inout] Contains the offset at which to download elements
   * and the number of elements to download. Is returned with the elements that
   * were available.
   */
  virtual void getMenuItems(MenuItems_t& menuItems) = 0;

  /**
   * \brief Callback to obtian the current known length of the menu.
   *
   * The length is not assumed to be constant for a given menu.
   */
  virtual MenuItemIndex_t getTotalMenuLength();

  /**
   * \brief Callback to trigger updating the display with new data.
   *
   * Causes getMenuItems to be called. Some menus may have to implement delayed
   * loading of items. Call this when new data is available for display.
   */
  // void updateMenuItems();

  virtual void notifyEncoderMoved(MenuItemIndex_t newItem);

  /// Protected destructor to prevent destruction through base class.
  ~AbstractMenu() = default;

 protected:
  MenuItemIndex_t getMenuItemInFirstDisplayLine() const {
    return menuItemInFirstDisplayLine;
  }

  EncoderLimiter limiter;

 private:
  /**
   * \brief The index of the menu item that is currently shown in the first line
   * of the display.
   */
  MenuItemIndex_t menuItemInFirstDisplayLine;

  /// The currently selected menu item.
  MenuItemIndex_t currentItem;

  /// Whether an action was taken that requires an update to the display.
  bool displayUpdateNeeded;
};

}  // namespace controller

#endif  // __CONTROLLER__ABSTRACTMENU_H__
