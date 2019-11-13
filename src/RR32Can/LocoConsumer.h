#ifndef SRC_RR32CAN_LOCOCONSUMER_H_
#define SRC_RR32CAN_LOCOCONSUMER_H_

#include <RR32Can/Locomotive.h>
#include <array>

#include "RR32Can/util/ConfigDataConsumer.h"

namespace RR32Can {

/*
 * \brief Class LocoConsumer
 *
 * Responsible for message handling from the master controller.
 */
class LocoConsumer : public RR32Can::ConfigDataConsumer {
 public:
  static const char* kFilenameEngine;

  virtual ~LocoConsumer() = default;

  bool isValid() const {
    return currentEngine != nullptr && currentEngine->isFullDetailsKnown() &&
           !currentEngine->isFree();
  }

  Locomotive* getEngine() { return currentEngine; }

  void setEngine(Locomotive* engine) { this->currentEngine = engine; }

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
  Locomotive* currentEngine;
  static const char* kFilenameEngineResult;
  static const char* kNoEngineSelected;
  static const char* kEngineKeyUid;
  static const char* kEngineKeyVelocity;
  static const char* kEngineKeyDirection;
  static const char* kEngineKeyProtocol;
  static const char* kEngineKeyAddress;
};

}  // namespace RR32Can

#endif  // SRC_RR32CAN_LOCOCONSUMER_H_
