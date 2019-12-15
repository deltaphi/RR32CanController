#ifndef __CONSOLEMANAGER_H__
#define __CONSOLEMANAGER_H__

#include "FreeRTOS.h"
#include "freertos/task.h"

/*
 * \brief Class ConsoleManager
 */
class ConsoleManager {
 public:
  void initialize_console(void);
  void StartTask();

 private:
  TaskHandle_t consoleTaskHandle;

  void setupCommands();
};

#endif  // __CONSOLEMANAGER_H__
