#include "RR32Can/util/ConfigDataStreamParser.h"

#include <Arduino.h>

#include <config.h>

namespace RR32Can {

void ConfigDataStreamParser::addMessage(const Data& data) {
  switch (streamState) {
    case StreamState::IDLE:
      Serial.println("Received uninteresting config data (IDLE).");
      break;
    case StreamState::STREAM_DONE:
      Serial.println("Received uninteresting config data (STREAM_DONE).");
      break;
    case StreamState::WAITING_FIRST_PACKET:
      // First packet has a length less than 8

      if (data.dlc == 6 || data.dlc == 7) {
        // Initial uncompressed
        remainingBytes = (data.data[0] << 24) | (data.data[1] << 16) |
                         (data.data[2] << 8) | (data.data[3]);
        uint16_t crc = (data.data[4] << 8) | (data.data[5]);
        this->crc.loadReference(crc);

#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_PACKETS)
        printf(
            " Stream length: %i Bytes. CRC requested: %#04x CRC actual: "
            "%#04x.\n",
            remainingBytes, crc, this->crc.getCrc());

        if (data.dlc == 7) {
          // Initial compressed
          printf("Compressed Data!\n");
        }
#endif

        if (remainingBytes > 0) {
          streamState = StreamState::WAITING_DATA_PACKET;
        } else {
          streamState = StreamState::STREAM_DONE;
        }

      } else if (data.dlc >= 8) {
        printf("Oversized first packet! Aborting parse.\n");
        reset();
        return;
      } else if (data.dlc < 6) {
        printf("Undersized first packet! Aborting parse.\n");
        reset();
        return;
      }
      break;
    case StreamState::WAITING_DATA_PACKET:
      if (data.dlc != 8) {
        printf("Undersized data packet! Aborting parse.\n");
        reset();
        return;
      } else {
        // regular data packet

#if (LOG_DUMP_CONFIG_DATA_STREAM == STD_ON)
        data.printAsText();
#endif

        remainingBytes -= data.dlc;
        for (int i = 0; i < data.dlc; ++i) {
          crc.updateCrc(data.data[i]);
        }

        // TODO: Remove this string copy. Requires BufferManager to be
        // constructible on a const char*
        char buffer[CanDataMaxLength];
        strncpy(buffer, data.dataAsString(), CanDataMaxLength);

        BufferManager input(buffer, data.dlc, CanDataMaxLength);
        textParser.addText(input);

        if (remainingBytes == 0) {
          if (crc.isCrcValid()) {
            streamState = StreamState::STREAM_DONE;
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_EVENTS)
            printf("Stream complete!\n");
#endif
            if (consumer != nullptr) {
              consumer->setStreamComplete();
            }

          } else {
            streamState = StreamState::IDLE;
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_EVENTS)
            printf(
                "CRC Error - Stream aborted. CRC requested: %#04x, CRC actual: "
                "%#04x.\n", crc.getReference(), crc.getCrc());
#endif
            if (consumer != nullptr) {
              consumer->setStreamAborted();
            }
          }
        } else {
#if (LOG_CONFIG_DATA_STREAM_LEVEL >= LOG_CONFIG_DATA_STREAM_LEVEL_PACKETS)
          printf("Stream continues. Bytes remaining: %i, Current CRC: %#04x\n",
                 remainingBytes, crc.getCrc());
#endif
        }
      }
      break;
  }
}

}  // namespace RR32Can
