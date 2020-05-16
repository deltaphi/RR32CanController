// Mock file for Unittesting

#ifndef __ESP_CONSOLE_H__
#define __ESP_CONSOLE_H__

#include <cstdint>

#define ESP_ERROR_CHECK(x) (x)

typedef int32_t esp_err_t;

typedef int (*esp_console_cmd_func_t)(int argc, char** argv);

typedef struct {
  const char* command;  //!< command name
  const char* help;
  const char* hint;
  esp_console_cmd_func_t func;
  void* argtable;
} esp_console_cmd_t;

inline esp_err_t esp_console_cmd_register(const esp_console_cmd_t* cmd) {
  return 0;
};

#endif  // __ESP_CONSOLE_H__