#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <cstdint>
#include "RR32Can/Constants.h"

#define STD_OFF (0U)
#define STD_ON (1U)

// Use 0 for "manual settings", 1 for first prototype, 2 for second prototype, 3 for control only
#define HARDWARE_VERSION (3)

#ifndef HARDWARE_VERSION
#error "You must set a Hardware verison. Use 0 if unsure."
#endif

#define DISPLAY_ATTACHED STD_ON
#define ENCODER_ENABLED STD_ON

#define S88_DATA_IN_PIN (18)
#define S88_CLOCK_OUT_PIN (23)
#define S88_PS_OUT_PIN (22)
#define S88_RESET_OUT_PIN (21)

#if (HARDWARE_VERSION == 3)
#define CAN_ENABLED STD_OFF
#else
#define CAN_ENABLED STD_ON
#endif

#if (CAN_ENABLED == STD_ON)
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
#endif // CAN_ENABLED

#if (ENCODER_ENABLED == STD_ON)
#if (HARDWARE_VERSION == 1 || HARDWARE_VERSION == 2)
#define ENCODER_A_PIN (35)
#define ENCODER_B_PIN (32)
#elif (HARDWARE_VERSION == 3)
#define ENCODER_A_PIN (18)
#define ENCODER_B_PIN (23)
#endif

// Should be 5
#define NUM_FBUTTONS (5)

#if (HARDWARE_VERSION == 3)
#define FBUTTONS_ON_SHIFTREGISTER STD_ON
#else
#define FBUTTONS_ON_SHIFTREGISTER STD_OFF
#endif

#if (FBUTTONS_ON_SHIFTREGISTER == STD_ON)
// Buttons on pins:
#define PIN_F0 (22)
#define PIN_F1 (05)
#define PIN_F2 (15)
#define PIN_F3 (21)
#define PIN_F4 (17)
constexpr const uint8_t kPinsFunction[NUM_FBUTTONS] = {PIN_F0, PIN_F1, PIN_F2, PIN_F3, PIN_F4};
#define PIN_SHIFT (04)
#define PIN_STOP (0)
#define PIN_ENCODER (16)
#define SHIFT_INVERTED STD_OFF
#else
#define SHIFT_INVERTED STD_ON
#endif

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
#endif

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

#define LOG_S88_BITS STD_OFF
#define LOG_BUTTON_PRESS STD_OFF
#define LOG_ACTIONLIST STD_OFF
#define LOG_BUTTON_MAPPING STD_OFF

#define LOG_CAN_IN_MSG STD_OFF
#define LOG_CAN_RAW_MSG_IN STD_OFF
#define LOG_CAN_OUT_MSG STD_OFF
#define LOG_PING STD_OFF

// Levels: 0 - off. 1 - log received elements. 2 - log received events. 10 - log
// every processing step.
#define LOG_CONFIG_DATA_STREAM_LEVEL_OFF (0)
#define LOG_CONFIG_DATA_STREAM_LEVEL_ELEMENTS (1)
#define LOG_CONFIG_DATA_STREAM_LEVEL_EVENTS (2)
#define LOG_CONFIG_DATA_STREAM_LEVEL_PACKETS (3)
#define LOG_CONFIG_DATA_STREAM_LEVEL_ALL (10)

#define LOG_CONFIG_DATA_STREAM_LEVEL (LOG_CONFIG_DATA_STREAM_LEVEL_ELEMENTS)
#define LOG_DUMP_CONFIG_DATA_STREAM STD_OFF

// Debouncing on is not tested
#define KEYS_DEBOUNCE STD_OFF
#define LOG_KEYS_DEBOUNCE STD_OFF

constexpr uint32_t RR32CanUUID = (RR32Can::enthusiastStationBaseUUID + 10);

static constexpr const uint8_t kActionListMaxLength = 6;

#endif