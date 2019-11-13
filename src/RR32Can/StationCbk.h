#ifndef __RR32CAN__STATIONCBK_H__
#define __RR32CAN__STATIONCBK_H__

#include <RR32Can/Locomotive.h>

namespace RR32Can {

/*
 * \brief Class StationCbk
 */
class StationCbk {
 public:
  /**
   * \brief Return the loco with the given Uid or nullptr, if the engine is not
   * known.
   */
  virtual Locomotive* getLoco(Locomotive::Uid_t uid) = 0;

  /**
   * \brief Set the velocity of the loco with the given UID.
   *
   * Should have no effect, if the engine is not known.
   */
  virtual void setLocoVelocity(Locomotive::Uid_t engineUid,
                               RR32Can::Velocity_t velocity) = 0;

  /**
   * \brief Unconditionally set the velocity of the currently controlled engine
   * to 0.
   */
  virtual void setLocoVelocity(RR32Can::Velocity_t velocity) = 0;

  /**
   * \brief Set whether the system is on (true) or off (false)
   */
  virtual void setSystemState(bool onOff) = 0;
};

}  // namespace RR32Can

#endif  // __RR32CAN__STATIONCBK_H__
