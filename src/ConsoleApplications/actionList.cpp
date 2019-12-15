#include "ConsoleApplications/actionList.h"

#include "argtable3/argtable3.h"
#include "esp_console.h"

#include <cstring>

#include "config.h"

namespace ConsoleApplications {
namespace ActionList {

static TurnoutControl::ActionListProcessor* actionListProcessor;

static const char* programName = "actionList";

static const char* kDump = "dump";
static const char* kExecute = "execute";
static const char* kSet = "set";

static arg_str* subcommand =
    arg_str1(nullptr, nullptr, "dump|execute|set", "Subcommand to execute");
static arg_int* actionListIndex =
    arg_int1(nullptr, nullptr, "uint8_t", "Index of ActionList");
static arg_int* actions =
    arg_intn(nullptr, nullptr, "TunoutAddr/Direction", 0, kActionListMaxLength,
             "List of turnouts to set");

struct arg_end* argEnd = arg_end(5);

static void* argtable[] = {subcommand, actionListIndex, actions, argEnd};

void Setup(TurnoutControl::ActionListProcessor& alp) {
  actionListProcessor = &alp;

  esp_console_cmd_t actuateTurnout{
    command : programName,
    help : "Actuate a turnout on request",
    hint : nullptr,
    func : ActionListMain,
    argtable : argtable
  };
  ESP_ERROR_CHECK(esp_console_cmd_register(&actuateTurnout));
}

int ActionListMain(int argc, char** argv) {
  int nerrors = arg_parse(argc, argv, argtable);
  int returncode = 0;
  if (nerrors != 0) {
    arg_print_errors(stdout, argEnd, programName);
    returncode = -1;
  } else {
    if (strncmp(subcommand->sval[0], kDump, strlen(kDump)) == 0) {
      return DumpActionLists(actionListIndex->ival[0]);
    } else if (strncmp(subcommand->sval[0], kExecute, strlen(kExecute)) == 0) {
      return ExecuteActionList(actionListIndex->ival[0]);
    } else if (strncmp(subcommand->sval[0], kSet, strlen(kSet)) == 0) {
      return SetActionList(actionListIndex->ival[0], actions);
    }
  }

  return returncode;
}

int checkListIndex(int listIndex) {
  if (listIndex < 0 || listIndex >= actionListProcessor->getNumActionLists()) {
    // out of range, parameter error
    return 1;
  } else {
    return 0;
  }
}

int DumpActionLists(int listIndex) {
  int listIndexCheck = checkListIndex(listIndex);
  if (listIndexCheck != 0) {
    return listIndexCheck;
  } else {
    model::TurnoutAction* al = TurnoutControl::actionLists[listIndex];
    for (int i = 0; i < model::NumActions; ++i) {
      printf("Turnout %i to %i.\n", al[i].address, static_cast<int>(al[i].direction));
    }
    return 0;
  }
}

int ExecuteActionList(int listIndex) {
  int listIndexCheck = checkListIndex(listIndex);
  if (listIndexCheck != 0) {
    return listIndexCheck;
  } else {
    bool result = actionListProcessor->requestActionList(listIndex);
    if (result) {
      return 0;
    } else {
      return 3;  // action did not work
    }
  }
}

int SetActionList(int listIndex, arg_int* actions) {
  int listIndexCheck = checkListIndex(listIndex);
  if (actions->count % 2 != 0) {
    printf("Turnout without action found.\n");
    return 1;
  } else if (listIndexCheck != 0) {
    return listIndexCheck;
  } else {
    return 2;  // TBD: not implemented
  }
}

}  // namespace ActionList
}  // namespace ConsoleApplications
