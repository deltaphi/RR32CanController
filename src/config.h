#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <cstdint>
#include "RR32Can/Constants.h"
#include "RR32Can_config.h"

// Use 0 for "manual settings", 1 for first prototype, 2 for second prototype, 3 for control only
#define HARDWARE_VERSION (1)

#ifndef HARDWARE_VERSION
#error "You must set a Hardware verison. Use 0 if unsure."
#endif

/*
 * Configuration of attached hardware
 */

#define DISPLAY_ATTACHED STD_ON
#define ENCODER_ENABLED STD_ON

#define S88_DATA_IN_PIN (18)
#define S88_CLOCK_OUT_PIN (23)
#define S88_PS_OUT_PIN (22)
#define S88_RESET_OUT_PIN (21)

#define CAN_MAX_DATA_LEN (8)

#define CAN_DRIVER_SJA1000 STD_OFF
#define CAN_DRIVER_ESP32IDF STD_ON

#if (CAN_DRIVER_SJA1000 == STD_ON) && (CAN_DRIVER_ESP32IDF == STD_ON)
#error Only one CAN driver can be active at any time.
#endif

// CAN pins use default driver values
#if CAN_DRIVER_SJA1000 == STD_ON
#define CAN_RX_PIN (4)
#define CAN_TX_PIN (5)
#else
#define CAN_RX_PIN (GPIO_NUM_4)
#define CAN_TX_PIN (GPIO_NUM_5)
#endif

#if (HARDWARE_VERSION == 1 || HARDWARE_VERSION == 2)
#define TWI_SCL_PIN (26)
#define TWI_SDA_PIN (27)
#elif (HARDWARE_VERSION == 3)
#define TWI_SCL_PIN (25)
#define TWI_SDA_PIN (26)
#endif

#if (DISPLAY_ATTACHED == STD_ON)
#define DISPLAY_TWI_ADDRESS (0x3C)

#if (HARDWARE_VERSION == 1 || HARDWARE_VERSION == 2)
#define DISPLAY_FLIP_SCREEN STD_OFF
#elif (HARDWARE_VERSION == 3)
#define DISPLAY_FLIP_SCREEN STD_ON
#endif

#define DISPLAY_CONTROLLER_SH1106 (1)
#define DISPLAY_CONTROLLER_SSD1306 (2)

#if (HARDWARE_VERSION == 1 || HARDWARE_VERSION == 3)
#define DISPLAY_CONTROLLER_TYPE DISPLAY_CONTROLLER_SSD1306
#elif (HARDWARE_VERSION == 2)
#define DISPLAY_CONTROLLER_TYPE DISPLAY_CONTROLLER_SH1106
#else
// Set manually here
#define DISPLAY_CONTROLLER_TYPE DISPLAY_CONTROLLER_SSD1306
#endif

#ifndef DISPLAY_CONTROLLER_TYPE
#error "Please define a display controller type"
#endif
#endif  // DISPLAY_ATTACHED

/*
 * Configuration of input buttons
 */

#if (ENCODER_ENABLED == STD_ON)
#if (HARDWARE_VERSION == 1 || HARDWARE_VERSION == 2)
#define ENCODER_A_PIN (35)
#define ENCODER_B_PIN (32)
#elif (HARDWARE_VERSION == 3)
#define ENCODER_A_PIN (18)
#define ENCODER_B_PIN (23)
#endif

#define NUM_FBUTTONS (5)
#endif  // ENCODER_ENABLED

#if (HARDWARE_VERSION == 1)
#define SHIFT_REGISTER_LENGTH (48U)
#define TURNOUT_BUTTONS_OFFSET (8)
#define TURNOUT_BUTTONS_COUNT (SHIFT_REGISTER_LENGTH - TURNOUT_BUTTONS_OFFSET)
#elif (HARDWARE_VERSION == 2)
#define SHIFT_REGISTER_LENGTH (64U)
#define TURNOUT_BUTTONS_OFFSET (8)
#define TURNOUT_BUTTONS_COUNT (SHIFT_REGISTER_LENGTH - TURNOUT_BUTTONS_OFFSET)
#elif (HARDWARE_VERSION == 3)
#define SHIFT_REGISTER_LENGTH (0U)
#define TURNOUT_BUTTONS_OFFSET (0)
#define TURNOUT_BUTTONS_COUNT (0U)
#else
// Set manually here
#define SHIFT_REGISTER_LENGTH (64U)
#define TURNOUT_BUTTONS_OFFSET (8)
#define TURNOUT_BUTTONS_COUNT (SHIFT_REGISTER_LENGTH - TURNOUT_BUTTONS_OFFSET)
#endif

/*
 * Buttons are:
 * Encoder
 * Shift
 * Stop
 * Function Keys
 *
 * They can be mapped to a GPIO or to a shift register bit or not be mapped at all.
 * Typically they are all mapped to GPIOs or all to shift registers.
 *
 * Make sure to set TURNOUT_BUTTONS_OFFSET accordingly!
 */
#define KEY_MODE_SHIFTREGISTER (1)
#define KEY_MODE_GPIO (2)

#if (HARDWARE_VERSION == 1 || HARDWARE_VERSION == 2)
#define FUNCTION_KEY_MODE KEY_MODE_SHIFTREGISTER
#elif (HARDWARE_VERION == 3)
#define FUNCTION_KEY_MODE KEY_MODE_GPIO
#else
// Define your own
#define FUNCTION_KEY_MODE KEY_MODE_GPIO
#endif

#if (FUNCTION_KEY_MODE == KEY_MODE_SHIFTREGISTER)
#define KEY_BIT_ENCODER (7)
#define KEY_BIT_SHIFT (5)
#define KEY_BIT_STOP (6)
#define KEY_BIT_F_OFFSET (0)
#define KEY_BIT_F_COUNT (NUM_FBUTTONS)
#endif

#if (FUNCTION_KEY_MODE == KEY_MODE_GPIO)
#define KEY_GPIO_ENCODER (16)
#define KEY_GPIO_SHIFT (04)
#define KEY_GPIO_STOP (0)
#define KEY_GPIO_F0 (22)
#define KEY_GPIO_F1 (05)
#define KEY_GPIO_F2 (15)
#define KEY_GPIO_F3 (21)
#define KEY_GPIO_F4 (17)
#endif

#if (SHIFT_REGISTER_LENGTH < NUM_TURNOUT_BITS)
#error "Shift Register is too short."
#endif

// Debouncing on is not tested
#define KEYS_DEBOUNCE STD_OFF
#define LOG_KEYS_DEBOUNCE STD_OFF

static constexpr const uint8_t kActionListMaxLength = 6;

#endif  // __CONFIG_H__