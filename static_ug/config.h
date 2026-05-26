#pragma once

#undef RGBLIGHT_LED_COUNT
#define RGBLIGHT_LED_COUNT 12

#undef RGBLIGHT_SPLIT_COUNT
#define RGBLIGHT_SPLIT_COUNT { 6, 6 }

// RGBLIGHT_SPLIT intentionally disabled — sync packets are unreliable on
// the RP2040 + elite_pi serial transport. Each half drives its own LEDs
// locally from the synced layer state instead (see layer_state_set_user).
// #define RGBLIGHT_SPLIT
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_LED_STATE_ENABLE

#define RGBLIGHT_DEFAULT_HUE 0
#define RGBLIGHT_DEFAULT_SAT 0
#define RGBLIGHT_DEFAULT_VAL 128
#define RGBLIGHT_DEFAULT_MODE RGBLIGHT_MODE_STATIC_LIGHT

#define RGBLIGHT_SLEEP

#undef RGBLIGHT_LIMIT_VAL
#define RGBLIGHT_LIMIT_VAL 180
