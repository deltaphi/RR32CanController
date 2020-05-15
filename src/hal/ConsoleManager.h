#ifndef __HAL_CONSOLEMANAGER_H__
#define __HAL_CONSOLEMANAGER_H__

#include "FreeRTOS.h"
#include "freertos/task.h"

#include "application/controller/ActionListControl.h"

/*
 * \brief Class ConsoleManager
 */
class ConsoleManager {
 public:
  void initialize_console(void);
  void StartTask();
  void setupCommands(application::model::ActionListModel& actionListModel,
                     application::controller::ActionListControl& actionListProcessor,
                     application::controller::ActionlistStorageCbk& scbk);

 private:
  TaskHandle_t consoleTaskHandle;
};

#endif  // __HAL_CONSOLEMANAGER_H__
