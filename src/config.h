#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <cstdint>
#include "RR32Can/Constants.h"

#define STD_OFF (0U)
#define STD_ON (1U)

#define DISPLAY_ATTACHED STD_ON
#define ENCODER_ENABLED STD_ON

#define S88_DATA_IN_PIN (18)
#define S88_CLOCK_OUT_PIN (23)
#define S88_PS_OUT_PIN (22)
#define S88_RESET_OUT_PIN (21)

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

#if (ENCODER_ENABLED == STD_ON)
#define ENCODER_A_PIN (35)
#define ENCODER_B_PIN (32)

// Should be 5
#define NUM_FBUTTONS (5)

#endif

#define TWI_SCL_PIN (26)
#define TWI_SDA_PIN (27)

#if (DISPLAY_ATTACHED == STD_ON)
#define DISPLAY_TWI_ADDRESS (0x3C)
#define DISPLAY_FLIP_SCREEN STD_OFF

#define DISPLAY_CONTROLLER_SH1106 (1)
#define DISPLAY_CONTROLLER_SSD1306 (2)

#define DISPLAY_CONTROLLER_TYPE DISPLAY_CONTROLLER_SSD1306

#ifndef DISPLAY_CONTROLLER_TYPE
#error "Please define a display controller type"
#endif
#endif

#define SHIFT_REGISTER_LENGTH (64U)

#define TURNOUT_BUTTONS_OFFSET (8)
#define TURNOUT_BUTTONS_COUNT (48)

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

#endif