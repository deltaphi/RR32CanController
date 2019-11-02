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

#if (LOG_CONFIG_DATA_STREAM_PROCESSING == STD_ON)
        Serial.print(" Stream length: ");
        Serial.print(remainingBytes, DEC);
        Serial.print(" Bytes. CRC requested: ");
        Serial.print(crc, HEX);
        Serial.print(" CRC actual: ");
        Serial.println(this->crc.getCrc(), HEX);

        if (data.dlc == 7) {
          // Initial compressed
          Serial.println(". Compressed Data!");
        }
#endif

        if (remainingBytes > 0) {
          streamState = StreamState::WAITING_DATA_PACKET;
        } else {
          streamState = StreamState::STREAM_DONE;
        }

      } else if (data.dlc >= 8) {
        Serial.println("Oversized first packet! Aborting parse.");
        reset();
        return;
      } else if (data.dlc < 6) {
        Serial.println("Undersized first packet! Aborting parse.");
        reset();
        return;
      }
      break;
    case StreamState::WAITING_DATA_PACKET:
      if (data.dlc != 8) {
        Serial.println("Undersized data packet! Aborting parse.");
        reset();
        return;
      } else {
        // regular data packet

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
#if (LOG_CONFIG_DATA_STREAM_PROCESSING == STD_ON)
            Serial.println("Stream complete!");
#endif
            if (consumer != nullptr) {
              consumer->setStreamComplete();
            }

          } else {
            streamState = StreamState::IDLE;
#if (LOG_CONFIG_DATA_STREAM_PROCESSING == STD_ON)
            Serial.print("CRC Error - Stream aborted. CRC requested: ");
            Serial.print(crc.getReference(), HEX);
            Serial.print(" CRC actual: ");
            Serial.println(crc.getCrc(), HEX);
#endif
            if (consumer != nullptr) {
              consumer->setStreamAborted();
            }
          }
        } else {
#if (LOG_CONFIG_DATA_STREAM_PROCESSING == STD_ON)
          Serial.print("Stream continues. Bytes remaining: ");
          Serial.print(remainingBytes, DEC);
          Serial.print(" Current CRC: ");
          Serial.println(crc.getCrc(), HEX);
#endif
        }
      }
      break;
  }
}

}  // namespace RR32Can
