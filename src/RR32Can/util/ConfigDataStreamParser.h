#ifndef SRC_RR32CAN_CONFIGDATASTREAMPARSER_H_
#define SRC_RR32CAN_CONFIGDATASTREAMPARSER_H_

#include "RR32Can/messages/Data.h"

#include "RR32Can/util/BufferManager.h"
#include "RR32Can/util/ConfigDataConsumer.h"
#include "RR32Can/util/Crc.h"
#include "RR32Can/util/TextParser.h"

namespace RR32Can {

/*
 * \brief Parser class for a complete ConfigData stream
 */
class ConfigDataStreamParser {
 public:
  enum class StreamState {
    IDLE = 0,
    WAITING_FIRST_PACKET,
    WAITING_DATA_PACKET,
    STREAM_DONE
  };

  void startStream(ConfigDataConsumer* consumer) {
    this->consumer = consumer;
    textParser.setConsumer(consumer);
    streamState = StreamState::WAITING_FIRST_PACKET;
    crc.reset();
  }

  void addMessage(const RR32Can::Data& data);

  void reset() {
    if (streamState == StreamState::WAITING_FIRST_PACKET ||
        streamState == StreamState::WAITING_DATA_PACKET) {
      if (consumer != nullptr) {
        consumer->setStreamAborted();
      }
    }

    streamState = StreamState::IDLE;
    remainingBytes = 0;
    textParser.reset();
    crc.reset();
    consumer = nullptr;
  }

  StreamState getStreamState() const { return streamState; }

  bool isProcessing() const {
    return streamState != StreamState::IDLE &&
           streamState != StreamState::STREAM_DONE;
  }

 private:
  StreamState streamState = StreamState::IDLE;
  uint32_t remainingBytes = 0;
  TextParser textParser;
  Crc crc;
  ConfigDataConsumer* consumer = nullptr;
};

}  // namespace RR32Can

#endif  // SRC_RR32CAN_CONFIGDATASTREAMPARSER_H_
