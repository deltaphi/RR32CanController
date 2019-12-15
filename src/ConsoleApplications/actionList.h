#ifndef __CONSOLEAPPLICATIONS__ACTIONLIST_H__
#define __CONSOLEAPPLICATIONS__ACTIONLIST_H__

#include "TurnoutControl/ActionListProcessor.h"

#include "argtable3/argtable3.h"

namespace ConsoleApplications {
namespace ActionList {

void Setup(TurnoutControl::ActionListProcessor& actionListProcessor);
int ActionListMain(int argc, char** argv);

int DumpActionLists(int listIndex);
int ExecuteActionList(int listIndex);
int SetActionList(int listIndex, arg_int* actions);

}  // namespace ActionList
}  // namespace ConsoleApplications

#endif  // __CONSOLEAPPLICATIONS__ACTIONLIST_H__
