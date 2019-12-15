#include "ConsoleApplications/switchTurnout.h"

#include "argtable3/argtable3.h"
#include "esp_console.h"

#include "RR32Can/RR32Can.h"

namespace ConsoleApplications {
namespace SwitchTurnout {

static const char* programName = "turnout";

static arg_int* turnoutNum =
    arg_int1(nullptr, nullptr, "positive int, 1..320", "Turnout Address");
static arg_int* direction =
    arg_int1(nullptr, nullptr, "0 or 1", "Turnout Direction");

struct arg_end* argEnd = arg_end(5);

static void* argtable[] = {turnoutNum, direction, argEnd};

void Setup() {
  esp_console_cmd_t actuateTurnout{
    command : programName,
    help : "Actuate a turnout on request",
    hint : nullptr,
    func : TurnoutMain,
    argtable : argtable
  };
  ESP_ERROR_CHECK(esp_console_cmd_register(&actuateTurnout));
}

int TurnoutMain(int argc, char** argv) {
  int nerrors = arg_parse(argc, argv, argtable);
  int returncode = 0;
  if (nerrors != 0) {
    printf("SwitchTurnout: %i paramter errors.\n", nerrors);
    arg_print_errors(stdout, argEnd, programName);
    returncode = -1;
  } else {
    RR32Can::TurnoutAddress_t turnoutAddress = *(turnoutNum->ival);
    if (turnoutAddress == 320) {
      turnoutAddress = 0;
    } else {
      turnoutAddress -= 1;
    }

    RR32Can::TurnoutDirection turnoutDirection =
        RR32Can::TurnoutDirectionFromIntegral(direction->ival[0]);

    RR32Can::RR32Can.SendAccessoryPacket(turnoutAddress, turnoutDirection, 1);
    delay(100);

    RR32Can::RR32Can.SendAccessoryPacket(turnoutAddress, turnoutDirection, 0);
  }

  return returncode;
}

}  // namespace SwitchTurnout
}  // namespace ConsoleApplications
