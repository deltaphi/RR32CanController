
#include "application/model/TurnoutTypes.h"

namespace application {
namespace model {

TurnoutAddressMode SwitchMode(TurnoutAddressMode mode) {
  switch (mode) {
    case TurnoutAddressMode::SingleTurnout:
      return TurnoutAddressMode::MultiTurnout;
    default:
      return TurnoutAddressMode::SingleTurnout;
  }
}

void print(const TurnoutLookupResult& result) {
  printf("Mode: %i, Address: %i", static_cast<int>(result.mode),
         RR32Can::HumanTurnoutAddress(result.address).value());
}

void print(const TurnoutAction& result) {
  printf(
      "Address: %i, Direction: %i (%s)\n",
      RR32Can::HumanTurnoutAddress(result.address).value(),
      static_cast<uint8_t>(result.direction),
      (result.direction == RR32Can::TurnoutDirection::RED ? "RED" : "GREEN"));
}

} /* namespace model */
}  // namespace application
