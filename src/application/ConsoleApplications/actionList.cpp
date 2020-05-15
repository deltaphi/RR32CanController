#include "application/ConsoleApplications/actionList.h"

#include "argtable3/argtable3.h"
#include "esp_console.h"

#include <cstring>

#include "config.h"

namespace application {
namespace ConsoleApplications {
namespace ActionList {

static application::model::ActionListModel* actionListModel;
static application::controller::ActionListControl* actionListProcessor;
static application::controller::ActionlistStorageCbk * storageCbk;

static const char* programName = "actionList";

static const char* kList = "list";
static const char* kExecute = "execute";
static const char* kSet = "set";
static const char* kSave = "save";

static arg_str* subcommand = arg_str1(nullptr, nullptr, "list|execute|set|save",
                                      "Subcommand to execute");
static arg_int* actionListIndex =
    arg_int1(nullptr, nullptr, "uint8_t", "Index of ActionList");
static arg_int* actions =
    arg_intn(nullptr, nullptr, "TunoutAddr/Direction", 0,
             kActionListMaxLength * 2, "List of turnouts to set");

struct arg_end* argEnd = arg_end(5);

static void* argtable[] = {subcommand, actionListIndex, actions, argEnd};

void Setup(application::model::ActionListModel& alm,
           application::controller::ActionListControl& alp,
           application::controller::ActionlistStorageCbk & scbk) {
  actionListModel = &alm;
  actionListProcessor = &alp;
  storageCbk = &scbk;

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
  printf("Command:");
  for (int i = 0; i < argc; ++i) {
    printf(" %s", argv[i]);
  }
  printf(";\n");
  int nerrors = arg_parse(argc, argv, argtable);
  int returncode = 0;
  if (nerrors != 0) {
    arg_print_errors(stdout, argEnd, programName);
    returncode = -1;
  } else {
    if (strncmp(subcommand->sval[0], kList, strlen(kList)) == 0) {
      return DumpActionLists();
    } else if (strncmp(subcommand->sval[0], kExecute, strlen(kExecute)) == 0) {
      return ExecuteActionList(actionListIndex->ival[0]);
    } else if (strncmp(subcommand->sval[0], kSet, strlen(kSet)) == 0) {
      printf("%i %i %i\n", subcommand->count, actionListIndex->count,
             actions->count);
      return SetActionList(actionListIndex->ival[0], actions);
    } else if (strncmp(subcommand->sval[0], kSave, strlen(kSave)) == 0) {
      return SaveActionLists();
    } else {
      printf("%s: Unknown command '%s'.\n", programName, subcommand->sval[0]);
      return 1;
    }
  }

  return returncode;
}

int checkListIndex(int listIndex) {
  if (listIndex < 0 || listIndex >= actionListModel->getNumActionLists()) {
    // out of range, parameter error
    return 1;
  } else {
    return 0;
  }
}

int DumpActionLists() {
  actionListModel->printActionLists(programName);
  return 0;
}

int ExecuteActionList(RR32Can::HumanTurnoutAddress humanListIndex) {
  RR32Can::MachineTurnoutAddress listIndex{humanListIndex};
  int listIndexCheck = checkListIndex(listIndex.value());
  if (listIndexCheck != 0) {
    return listIndexCheck;
  } else {
    bool result = actionListProcessor->requestActionList(listIndex.value());
    if (result) {
      return 0;
    } else {
      return 3;  // action did not work
    }
  }
}

int SetActionList(RR32Can::HumanTurnoutAddress humanListIndex,
                  arg_int* actions) {
  RR32Can::MachineTurnoutAddress listIndex{humanListIndex};

  if (actions->count % 2 != 0) {
    printf("Turnout without action found.\n");
    return 1;
  } else {
    printf("ActionList with %i parameters.\n", actions->count);

    application::model::ActionListModel::DB_t& db = actionListModel->getDb();

    // Find or create the ActionList to be edited.

    if (listIndex.value() >= db.size()) {
      // create one (or more) new entries
      db.resize(listIndex.value() + 1);
    }

    application::model::ActionListModel::DB_t::iterator dbIt = db.begin();
    std::advance(dbIt, listIndex.value());

    // Clear the list and fill it with new entries
    dbIt->clear();

    for (int i = 0; i < actions->count; i += 2) {
      model::TurnoutAction action;
      action.address = RR32Can::HumanTurnoutAddress(actions->ival[i]);
      action.direction =
          RR32Can::TurnoutDirectionFromIntegral(actions->ival[i + 1]);
      dbIt->push_back(action);
    }

    printf("ActionList with %i elements stored.\n", dbIt->size());
    return 0;
  }
}

int SaveActionLists() {
  printf("Saving ActionLists to SPIFFS.\n");
  storageCbk->store(actionListModel->getDb());
  return 0;
}

}  // namespace ActionList
}  // namespace ConsoleApplications
}  // namespace application
