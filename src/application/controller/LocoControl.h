#ifndef __APPLICATION__CONTROLLER__LOCOCONTROL_H__
#define __APPLICATION__CONTROLLER__LOCOCONTROL_H__

#include <RR32Can/Locomotive.h>
#include <RR32CanEngineDb/LocoConsumer.h>
#include <RR32CanEngineDb/util/ConfigDataStreamParser.h>

#include "application/model/DisplayModel.h"
#include "application/model/InputState.h"

namespace application {
namespace controller {

class MasterControl;

/*
 * \brief Class LocoControl
 */
class LocoControl {
 public:
  void begin();
  void loop(application::model::InputState& inputState, MasterControl& masterControl);
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

  void enterLocoControl(application::model::InputState& inputState) {
    inputState.loadEncoderPosition(loco.getVelocity());
  }

  // Update the display text on a state change
  void updateDisplayOnce(application::model::DisplayModel& displayManager);

  // Update loco-related non-text display assets
  void updateDisplayLoop(application::model::DisplayModel& displayManager);

  // Update the engine velocity and set the encoder appropriately.
  void setReceivedVelocity(RR32Can::Velocity_t velocity, MasterControl& uiControl);

  // Get outside access to the engine. Don't set the velocity this way!
  RR32Can::Locomotive& getLoco() { return loco; }

 private:
  void checkEncoder(application::model::InputState& inputState);

  RR32Can::Locomotive loco;

  RR32Can::LocoConsumer locoDataConsumer;
  RR32Can::ConfigDataStreamParser streamParser;
};

}  // namespace controller
}  // namespace application

#endif  // __APPLICATION__CONTROLLER__LOCOCONTROL_H__
