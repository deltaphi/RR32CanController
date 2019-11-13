#ifndef __CONTROLLER__LOCOCONTROL_H__
#define __CONTROLLER__LOCOCONTROL_H__

#include <RR32Can/LocoConsumer.h>
#include <RR32Can/Locomotive.h>
#include "model/InputState.h"
#include "view/DisplayManager.h"

namespace controller {

class MasterControl;

/*
 * \brief Class LocoControl
 */
class LocoControl {
 public:
  void begin();
  void loop(model::InputState& inputState, MasterControl& masterControl);
  void reset();

  /**
   * \brief Set the contained loco info to the given loco.
   *
   * Checks whether the name of the engine is different.
   * If the name is not different, no change is made.
   *
   * \return true if the loco was updated, false otherwise.
   */
  bool setLocoInfo(const RR32Can::LocomotiveShortInfo& locoInfo);

  /**
   * \brief Given the name is known, request the "lokomotive" information from
   * the master.
   */
  void requestLocoFile();

  /**
   * \brief Given the UID is known, request control status info (speed,
   * direction, functions) from the master.
   */
  void requestLocoData();

  bool hasValidEngine() const { return loco.isFullDetailsKnown(); }

  void enterLocoControl(model::InputState& inputState) {
    inputState.loadEncoderPosition(loco.getVelocity());
  }

  // Update the display text on a state change
  void updateDisplayOnce(view::DisplayManager& displayManager);

  // Update loco-related non-text display assets
  void updateDisplayLoop(view::DisplayManager& displayManager);

  // Update the engine velocity and set the encoder appropriately.
  void setReceivedVelocity(RR32Can::Velocity_t velocity,
                           MasterControl& uiControl);

  // Get outside access to the engine. Don't set the velocity this way!
  RR32Can::Locomotive& getLoco() { return loco; }

 private:
  void checkEncoder(model::InputState& inputState);

  RR32Can::Locomotive loco;

  RR32Can::LocoConsumer locoDataConsumer;
};

}  // namespace controller

#endif  // __CONTROLLER__LOCOCONTROL_H__
