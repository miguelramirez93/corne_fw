#pragma once

// Each half is treated as a plain 6-LED chain with no split-RGB logic at all.
// We rely on SPLIT_LAYER_STATE_ENABLE to sync the layer variable, then each
// half independently drives its own LEDs from housekeeping_task_user.
#undef RGBLIGHT_LED_COUNT
#define RGBLIGHT_LED_COUNT 6

// RGBLIGHT_SPLIT_COUNT and RGBLIGHT_SPLIT intentionally NOT defined — they
// activate split-aware RGB logic that proved unreliable on this RP2040 build.
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_LED_STATE_ENABLE

#define RGBLIGHT_DEFAULT_HUE 0
#define RGBLIGHT_DEFAULT_SAT 0
#define RGBLIGHT_DEFAULT_VAL 128
#define RGBLIGHT_DEFAULT_MODE RGBLIGHT_MODE_STATIC_LIGHT

#define RGBLIGHT_SLEEP

#undef RGBLIGHT_LIMIT_VAL
#define RGBLIGHT_LIMIT_VAL 180
