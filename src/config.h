#ifndef __CONFIG_H__
#define __CONFIG_H__

#define STD_OFF (0U)
#define STD_ON (1U)

#define DISPLAY_ATTACHED STD_ON
#define ENCODER_ENABLED STD_OFF

#define S88_DATA_IN_PIN (18)
#define S88_CLOCK_OUT_PIN (23)
#define S88_PS_OUT_PIN (22)
#define S88_RESET_OUT_PIN (21)

#if (ENCODER_ENABLED == STD_ON)
#define ENCODER_LEFT_PIN (34)
#define ENCODER_RIGHT_PIN (33)
#define ENCODER_BUTTON_PIN (32)
#endif

#define TWI_SCL_PIN (26)
#define TWI_SDA_PIN (27)

#if (DISPLAY_ATTACHED == STD_ON)
#define DISPLAY_TWI_ADDRESS (0x3C)
#endif

#define SHIFT_REGISTER_LENGTH (48U)

#define LOG_S88_BITS STD_OFF
#define LOG_BUTTON_PRESS STD_ON
#define LOG_ACTIONLIST STD_OFF

#define LOG_CAN_IN_MSG STD_OFF
#define LOG_CAN_OUT_MSG STD_ON
#define LOG_BUTTON_MAPPING STD_OFF

// Debouncing on is not tested
#define KEYS_DEBOUNCE STD_OFF
#define LOG_KEYS_DEBOUNCE STD_OFF

constexpr uint32_t maeklinPrivateBaseUUID = 0x00001800;
constexpr uint32_t maerklinCanUUID = (maeklinPrivateBaseUUID + 10);

#endif