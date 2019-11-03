#ifndef SRC_RR32CAN_ENGINEBROWSER_H_
#define SRC_RR32CAN_ENGINEBROWSER_H_

#include <array>

#include "RR32Can/Engine.h"
#include "RR32Can/util/ConfigDataConsumer.h"

namespace RR32Can {

/*
 * \brief Class EngineBrowser
 * 
 * Responsible for message handling from/to the master controller.
 */
class EngineBrowser : public ConfigDataConsumer {
 public:
  using EngineInfoSet = std::array<EngineShortInfo, kEngineBrowserEntries>;

  EngineBrowser() = default;
  EngineBrowser(const EngineBrowser&) = delete;
  EngineBrowser& operator=(const EngineBrowser&) = delete;
  EngineBrowser(EngineBrowser&&) = delete;
  EngineBrowser& operator=(EngineBrowser&&) = delete;
  ~EngineBrowser() = default;

  void setStreamOffset(uint8_t offset) { this->cursor = offset; }

  uint8_t getStreamOffset() const { return cursor; }

  void consumeConfigData(BufferManager& section, BufferManager& key,
                         BufferManager& value) override;
  void setStreamComplete() override { streamComplete = true; }
  void setStreamAborted() override { reset(); }

  void reset();

  bool isStreamComplete() const { return streamComplete; }

  void printAll() const;

  const EngineInfoSet& getEngineInfos() const { return engineInfo; }

  uint8_t getNumEnginesKnownByMaster() const { return numEnginesKnownByMaster; }

  const char* getCursorEngineName() const {
    Serial.print(" Cursor: ");
    Serial.print(cursor, DEC);
    Serial.print(" engineInfo.size(): ");
    Serial.print(engineInfo.size(), DEC);
    Serial.print(" Index: ");
    Serial.println(cursor % engineInfo.size(), DEC);
    return engineInfo[cursor % engineInfo.size()].getName();
  }

  //void fetchEngine(Engine& engine);

  static const char* kFilenameEngineNames;
  static const char* kSectionNumEngines;
  static const char* kGenericValue;

 private:
  bool streamComplete;
  EngineInfoSet engineInfo;
  uint8_t numEnginesKnownByMaster;

  /// Offset at which the download was started
  uint8_t cursor;

  EngineShortInfo* findFirstFreeEntry() {
    for (EngineShortInfo& info : engineInfo) {
      if (info.isFree()) {
        return &info;
      }
    }
    return nullptr;
  }
};

}  // namespace RR32Can

#endif  // SRC_RR32CAN_ENGINEBROWSER_H_
