#include "ConsoleManager.h"

#include "driver/uart.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "linenoise/linenoise.h"

void ConsoleManager::initialize_console(void) {
  /* Disable buffering on stdin */
  setvbuf(stdin, NULL, _IONBF, 0);

  /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
  esp_vfs_dev_uart_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
  /* Move the caret to the beginning of the next line on '\n' */
  esp_vfs_dev_uart_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

  /* Configure UART. Note that REF_TICK is used so that the baud rate remains
   * correct while APB frequency is changing in light sleep mode.
   */
  const uart_config_t uart_config = {
      .baud_rate = 115200 /*CONFIG_ESP_CONSOLE_UART_BAUDRATE*/,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .rx_flow_ctrl_thresh = 0,
      .use_ref_tick = true};

  ESP_ERROR_CHECK(uart_param_config(UART_NUM_0 /*CONFIG_ESP_CONSOLE_UART_NUM*/,
                                    &uart_config));

  /* Install UART driver for interrupt-driven reads and writes */
  ESP_ERROR_CHECK(uart_driver_install(
      UART_NUM_0 /*CONFIG_ESP_CONSOLE_UART_NUM*/, 256, 0, 0, NULL, 0));

  /* Tell VFS to use UART driver */
  esp_vfs_dev_uart_use_driver(UART_NUM_0 /*CONFIG_ESP_CONSOLE_UART_NUM*/);

  /* Initialize the console */
  esp_console_config_t console_config = {
    .max_cmdline_length = 256,
    .max_cmdline_args = 8,
#if CONFIG_LOG_COLORS
    .hint_color = atoi(LOG_COLOR_CYAN)
#endif
  };
  ESP_ERROR_CHECK(esp_console_init(&console_config));

  /* Configure linenoise line completion library */
  /* Enable multiline editing. If not set, long commands will scroll within
   * single line.
   */
  // linenoiseSetMultiLine(1);

  /* Tell linenoise where to get command completions and hints */
  // linenoiseSetCompletionCallback(&esp_console_get_completion);
  // linenoiseSetHintsCallback((linenoiseHintsCallback*) &esp_console_get_hint);

  /* Set command history size */
  linenoiseHistorySetMaxLen(100);

#if CONFIG_STORE_HISTORY
  /* Load command history from filesystem */
  linenoiseHistoryLoad(HISTORY_PATH);
#endif
}

void ConsoleTask(void* parameter) {
  /* Figure out if the terminal supports escape sequences */
  int probe_status = linenoiseProbe();
  if (probe_status) { /* zero indicates success */
    printf(
        "\n"
        "Your terminal application does not support escape sequences.\n"
        "Line editing and history features are disabled.\n"
        "On Windows, try using Putty instead.\n");
    linenoiseSetDumbMode(1);
#if CONFIG_LOG_COLORS
    /* Since the terminal doesn't support escape sequences,
     * don't use color codes in the prompt.
     */
    prompt = "esp32> ";
#endif  // CONFIG_LOG_COLORS
  }

  while (true) {
    const char* prompt = "ESP32> ";
    char* line = linenoise(prompt);
    if (line != NULL) {
      printf("Got a line: '%s'.\n", line);
      linenoiseFree(line);
    }
  }
}

void ConsoleManager::StartTask() {
  xTaskCreate(ConsoleTask, "ConsoleTask", 10000, nullptr, 0,
              &consoleTaskHandle);
}
