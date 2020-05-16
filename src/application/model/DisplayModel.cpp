#include "application/model/DisplayModel.h"

#include <cstdio>
#include <cstring>

namespace application {
namespace model {

void DisplayModel::begin() {
  printf("Starting Display Model\n");

  memset(buffer, 0, sizeof(buffer));
  setSpeedValue(0);
  setDirection(RR32Can::EngineDirection::UNKNOWN);
  setFunctionBits(0u);
  updateRequired = true;
}

void DisplayModel::updateBuffer(const char* data, uint8_t dataLen,
                                uint8_t lineNumber) {
  char* buffer = getWritableBuffer(lineNumber);

  if (dataLen > STRING_DATATYPE_LENGTH) {
    dataLen = STRING_DATATYPE_LENGTH;
  }

  if (strncmp(buffer, data, dataLen) != 0) {
    strncpy(buffer, data, dataLen);
    updateRequired = true;
  }  // else: No update needed
}

}  // namespace model
}  // namespace application
