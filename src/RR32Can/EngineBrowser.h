#ifndef SRC_RR32CAN_ENGINEBROWSER_H_
#define SRC_RR32CAN_ENGINEBROWSER_H_

#include <array>

#include "RR32Can/ConfigDataConsumer.h"
#include "RR32Can/Engine.h"

namespace RR32Can {

/*
 * \brief Class EngineBrowser
 */
class EngineBrowser : public ConfigDataConsumer {
 public:
  void consumeConfigData(BufferManager& section, BufferManager& key,
                         BufferManager& value) override;
  void setStreamComplete() override { streamComplete = true; }
  void setStreamAborted() override { reset(); }

  void reset();

  bool isStreamComplete() const { return streamComplete; }

  void printAll() const;

  static const char* kFilenameEngineNames;
  static const char* kSectionNumEngines;
  static const char* kGenericValue;

 private:
  bool streamComplete;
  std::array<EngineShortInfo, kEngineBrowserEntries> engineInfo;
  uint8_t numEnginesKnownByMaster;

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
