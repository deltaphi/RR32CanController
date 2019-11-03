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

// CAN pins use default driver values
// #define CAN_RX_PIN (4)
// #define CAN_TX_PIN (5)

#if (ENCODER_ENABLED == STD_ON)
#define ENCODER_A_PIN (32)
#define ENCODER_B_PIN (35)
#define ENCODER_BUTTON_PIN (34)

// Should be 5
#define NUM_FBUTTONS (4)

#define STOP_BUTTON_PIN (19)
// F0 should be at 2, but that pin doesn't work for some reason.
//#define F0_BUTTON_PIN (2)
// Use F0 at 14 instead.
#define F0_BUTTON_PIN (14)
#define F1_BUTTON_PIN (15)
#define F2_BUTTON_PIN (33)
#define F3_BUTTON_PIN (25)
// Temporarily disabled to reuse the pin for F0
//#define F4_BUTTON_PIN (14)
#define SHIFT_BUTTON_PIN (13)
#endif

#define TWI_SCL_PIN (26)
#define TWI_SDA_PIN (27)

#if (DISPLAY_ATTACHED == STD_ON)
#define DISPLAY_TWI_ADDRESS (0x3C)
#define DISPLAY_FLIP_SCREEN STD_OFF
#endif

#define SHIFT_REGISTER_LENGTH (48U)

#define LOG_S88_BITS STD_OFF
#define LOG_BUTTON_PRESS STD_ON
#define LOG_ACTIONLIST STD_OFF

#define LOG_CAN_IN_MSG STD_OFF
#define LOG_CAN_RAW_MSG_IN STD_OFF
#define LOG_CAN_OUT_MSG STD_ON
#define LOG_BUTTON_MAPPING STD_OFF

#define LOG_CONFIG_DATA_STREAM_PROCESSING STD_OFF

// Debouncing on is not tested
#define KEYS_DEBOUNCE STD_OFF
#define LOG_KEYS_DEBOUNCE STD_OFF

constexpr uint32_t RR32CanUUID = (RR32Can::enthusiastStationBaseUUID + 10);

#endif