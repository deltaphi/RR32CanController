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

  void setStreamOffset(uint8_t offset) { this->streamOffset = offset; }

  uint8_t getStreamOffset() const { return streamOffset; }

  void consumeConfigData(BufferManager& section, BufferManager& key,
                         BufferManager& value) override;
  void setStreamComplete() override { streamComplete = true; }
  void setStreamAborted() override { reset(); }

  void reset();
  void clearTable();

  bool isStreamComplete() const { return streamComplete; }

  void printAll() const;

  const EngineInfoSet& getEngineInfos() const { return engineInfo; }

  uint8_t getNumEnginesKnownByMaster() const { return numEnginesKnownByMaster; }

  // void fetchEngine(Engine& engine);

  static const char* kFilenameEngineNames;
  static const char* kSectionNumEngines;
  static const char* kGenericValue;

 private:
  bool streamComplete;
  EngineInfoSet engineInfo;
  uint8_t numEnginesKnownByMaster;

  /**
   * \brief Offset at which the download was started
   *
   * The streamOffset is an index into the engine list of the bus master.
   * It points to the entry that is currently displayed at the top of the list.
   */
  uint8_t streamOffset;

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
