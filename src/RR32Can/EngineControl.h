#ifndef SRC_RR32CAN_ENGINECONTROL_H_
#define SRC_RR32CAN_ENGINECONTROL_H_

#include <array>

#include "RR32Can/Engine.h"

namespace RR32Can {

/*
 * \brief Class EngineControl
 * 
 * Responsible for message handling from/to the master controller.
 */
class EngineControl {
 public:
  bool isValid() const {
    return currentEngine.isFullDetailsKnown() && !currentEngine.isFree();
  }

  Engine& getEngine() {
    return currentEngine;
  }

  const char* getEngineName() const {
    if (isValid()) {
      return currentEngine.getName();
    } else {
      return kNoEngineSelected;
    }
  }

 private:
  Engine currentEngine;
  static const char* kNoEngineSelected;
};

}  // namespace RR32Can

#endif  // SRC_RR32CAN_ENGINECONTROL_H_
