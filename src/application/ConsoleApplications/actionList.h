#ifndef __APPLICATION__CONSOLEAPPLICATIONS__ACTIONLIST_H__
#define __APPLICATION__CONSOLEAPPLICATIONS__ACTIONLIST_H__

#include "TurnoutControl/ActionListProcessor.h"
#include "application/controller/ActionlistStorageCbk.h"

#include "argtable3/argtable3.h"

namespace application {
namespace ConsoleApplications {
namespace ActionList {

void Setup(TurnoutControl::ActionListProcessor& actionListProcessor, application::controller::ActionlistStorageCbk & storageCbk);
int ActionListMain(int argc, char** argv);

int DumpActionLists();
int ExecuteActionList(RR32Can::HumanTurnoutAddress listIndex);
int SetActionList(RR32Can::HumanTurnoutAddress listIndex, arg_int* actions);
int SaveActionLists();

}  // namespace ActionList
}  // namespace ConsoleApplications
}  // namespace application

#endif  // __APPLICATION__CONSOLEAPPLICATIONS__ACTIONLIST_H__