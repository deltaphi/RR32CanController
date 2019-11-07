#ifndef SRC_RR32CAN_ENGINECONTROL_H_
#define SRC_RR32CAN_ENGINECONTROL_H_

#include <array>

#include "RR32Can/Engine.h"
#include "RR32Can/util/ConfigDataConsumer.h"

namespace RR32Can {

/*
 * \brief Class EngineControl
 *
 * Responsible for message handling from/to the master controller.
 */
class EngineControl : public RR32Can::ConfigDataConsumer {
 public:
  static const char* kFilenameEngine;

  virtual ~EngineControl() = default;

  bool isValid() const {
    return currentEngine.isFullDetailsKnown() && !currentEngine.isFree();
  }

  Engine& getEngine() { return currentEngine; }

  /**
   * \brief Returns the engine name if known or a default if unknown.
   * \return Name of the engine or a default string.
   */
  const char* getEngineName() const {
    if (isValid()) {
      return currentEngine.getName();
    } else {
      return kNoEngineSelected;
    }
  }

  /* Code for parsing Engine Parsing from Config Data Stream Code */
  virtual void consumeConfigData(BufferManager& section, BufferManager& key,
                                 BufferManager& value);

  /**
   * \brief Notify that the stream is complete.
   *
   * Sets the engine details to "fully known".
   */
  virtual void setStreamComplete();

  /**
   * \brief On abort, nothing to do.
   */
  virtual void setStreamAborted();

 private:
  Engine currentEngine;
  static const char* kFilenameEngineResult;
  static const char* kNoEngineSelected;
  static const char* kEngineKeyUid;
  static const char* kEngineKeyVelocity;
  static const char* kEngineKeyDirection;
  static const char* kEngineKeyProtocol;
  static const char* kEngineKeyAddress;
};

}  // namespace RR32Can

#endif  // SRC_RR32CAN_ENGINECONTROL_H_
