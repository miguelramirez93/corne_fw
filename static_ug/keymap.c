#include QMK_KEYBOARD_H
#if __has_include("keymap.h")
#    include "keymap.h"
#endif
#include "os_detection.h"
#include "send_string.h"
#include "ws2812.h"
enum custom_keycodes {
    SS_SEL = SAFE_RANGE,
};

static uint32_t master_last_activity = 0;
static uint32_t slave_last_activity = 0;
static uint32_t bark_until = 0;
#define MASTER_OLED_TIMEOUT_MS 60000
#define SLAVE_OLED_TIMEOUT_MS 60000
#define BARK_IDLE_THRESHOLD_MS 2000
#define BARK_DURATION_MS 1000

void matrix_scan_user(void) {
    if (!is_keyboard_master()) {
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            if (matrix_get_row(row) != 0) {
                slave_last_activity = timer_read32();
                if (!is_oled_on()) oled_on();
                return;
            }
        }
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        if (timer_elapsed32(master_last_activity) > BARK_IDLE_THRESHOLD_MS) {
            bark_until = timer_read32() + BARK_DURATION_MS;
        }
        master_last_activity = timer_read32();
        if (!is_oled_on()) {
            oled_on();
        }
    }
    if (record->event.pressed && keycode == SS_SEL) {
        switch (detected_host_os()) {
            case OS_WINDOWS:
                SEND_STRING(SS_LGUI(SS_LSFT("s")));
                break;
            case OS_LINUX:
                SEND_STRING(SS_LSFT(SS_TAP(X_PSCR)));
                break;
            case OS_MACOS:
            case OS_IOS:
            case OS_UNSURE:
            default:
                SEND_STRING(SS_LGUI(SS_LSFT("4")));
                break;
        }
        return false;
    }
    return true;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_split_3x6_3(KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSPC, KC_LCTL, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_ESC, KC_LGUI, MO(1), KC_SPC, KC_ENT, MO(2), KC_RALT),
    [1] = LAYOUT_split_3x6_3(KC_TAB, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_BSPC, KC_LCTL, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_NO, KC_NO, KC_LSFT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_LGUI, KC_TRNS, KC_SPC, KC_ENT, MO(3), KC_RALT),
    [2] = LAYOUT_split_3x6_3(KC_TAB, KC_EXLM, KC_AT, KC_HASH, KC_DLR, KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_BSPC, KC_LCTL, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_MINS, KC_EQL, KC_LBRC, KC_RBRC, KC_BSLS, KC_GRV, KC_LSFT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE, KC_TILD, KC_LGUI, MO(3), KC_SPC, KC_ENT, KC_TRNS, KC_RALT),
    [3] = LAYOUT_split_3x6_3(KC_F1,  KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_NO,    KC_NO,   SS_SEL,  KC_NO,   KC_NO, KC_NO, KC_MUTE, KC_MPRV, KC_MPLY, KC_MNXT, KC_NO, KC_NO, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO, QK_BOOT, KC_NO, KC_VOLD, KC_VOLU, KC_NO, KC_NO, KC_NO, KC_LGUI, KC_TRNS, KC_SPC, KC_ENT, KC_TRNS, KC_RALT)
};

void keyboard_post_init_user(void) {
    ws2812_set_color_all(0, 0, 0);
    ws2812_flush();
}

static void force_leds(uint8_t r, uint8_t g, uint8_t b) {
    ws2812_set_color_all(r, g, b);
    ws2812_flush();
}

void housekeeping_task_user(void) {
    static uint8_t last_layer = 0xFF;
    static bool last_caps = true;
    uint8_t layer = get_highest_layer(layer_state);
    bool caps = host_keyboard_led_state().caps_lock;
    if (layer != last_layer || caps != last_caps) {
        last_layer = layer;
        last_caps = caps;
        if (caps) {
            force_leds(0, 80, 0);
        } else {
            switch (layer) {
                case 1: force_leds(0, 80, 80); break;
                case 2: force_leds(60, 0, 80); break;
                case 3: force_leds(80, 0, 0); break;
                default: force_leds(0, 0, 0); break;
            }
        }
        if (!is_keyboard_master()) {
            slave_last_activity = timer_read32();
            if (!is_oled_on()) oled_on();
        }
    }
    if (is_keyboard_master() && is_oled_on() &&
        timer_elapsed32(master_last_activity) > MASTER_OLED_TIMEOUT_MS) {
        oled_off();
    }
    if (!is_keyboard_master() && is_oled_on() &&
        timer_elapsed32(slave_last_activity) > SLAVE_OLED_TIMEOUT_MS) {
        oled_off();
    }
}

#ifdef OLED_ENABLE

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_270;
}

static const char PROGMEM dog_frame_a[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x1c, 0x02, 0x05, 0x02, 0x24, 0x04, 0x04, 0x02, 0xa9, 0x1e, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x90, 0x08, 0x18, 0x60, 0x10, 0x08, 0x04, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0e, 0x82, 0x7c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x04, 0x0c, 0x10, 0x10, 0x20, 0x20, 0x20, 0x28, 0x3e, 0x1c, 0x20, 0x20, 0x3e, 0x0f, 0x11, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const char PROGMEM dog_frame_b[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x1c, 0x02, 0x05, 0x02, 0x24, 0x04, 0x04, 0x02, 0xa9, 0x1e, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x48, 0x04, 0x0c, 0x30, 0x08, 0x04, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0e, 0x82, 0x7c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x04, 0x0c, 0x10, 0x10, 0x20, 0x20, 0x20, 0x28, 0x3e, 0x1c, 0x20, 0x20, 0x3e, 0x0f, 0x11, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 32x32 bulldog, row-major MSB-first per byte (4 bytes per row)
static const uint8_t PROGMEM bulldog_pixels[128] = {
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x06, 0x00, 0x00, 0xC0,
    0x19, 0x80, 0x03, 0x20,
    0x60, 0x70, 0x3C, 0x18,
    0x4F, 0x0E, 0xC3, 0xE4,
    0x48, 0x00, 0x00, 0x24,
    0x48, 0x00, 0x00, 0x24,
    0x48, 0x00, 0x00, 0x28,
    0x54, 0x00, 0x00, 0x68,
    0x38, 0x00, 0x00, 0x58,
    0x09, 0xE4, 0x0F, 0x00,
    0x08, 0x1C, 0x70, 0x40,
    0x04, 0x00, 0x00, 0x40,
    0x04, 0x03, 0x00, 0x40,
    0x08, 0x0C, 0xC0, 0x40,
    0x08, 0x10, 0x30, 0x20,
    0x08, 0x20, 0x10, 0x20,
    0x00, 0x00, 0x00, 0x20,
    0x10, 0x00, 0x00, 0x20,
    0x10, 0x07, 0xC0, 0x20,
    0x10, 0x08, 0x60, 0x20,
    0x10, 0x10, 0x30, 0x20,
    0x08, 0x10, 0x10, 0x20,
    0x08, 0x30, 0x38, 0x20,
    0x04, 0x28, 0x68, 0x40,
    0x06, 0x47, 0xCC, 0x80,
    0x03, 0x80, 0x07, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

static void render_master(void) {
    if (!is_oled_on()) return;
    static uint32_t anim_timer = 0;
    static uint8_t frame_idx = 0;
    uint8_t wpm = get_current_wpm();
    uint16_t interval = (wpm == 0) ? 600 : (wpm < 30 ? 350 : 150);
    if (timer_elapsed32(anim_timer) > interval) {
        anim_timer = timer_read32();
        frame_idx ^= 1;
    }
    oled_set_cursor(0, 0);
    oled_write_raw_P(frame_idx == 0 ? dog_frame_a : dog_frame_b, sizeof(dog_frame_a));

    oled_set_cursor(0, 3);
    if (timer_elapsed32(bark_until) < BARK_DURATION_MS && timer_read32() < bark_until) {
        oled_write_P(PSTR("WOOF!"), false);
    } else {
        oled_write_P(PSTR("     "), false);
    }

    oled_set_cursor(0, 4);
    oled_write_P(PSTR("Layer"), false);
    oled_set_cursor(0, 5);
    switch (get_highest_layer(layer_state)) {
        case 0: oled_write_P(PSTR("BASE "), false); break;
        case 1: oled_write_P(PSTR("NAV  "), false); break;
        case 2: oled_write_P(PSTR("SYM  "), false); break;
        case 3: oled_write_P(PSTR("FN   "), false); break;
        default: oled_write_P(PSTR("???  "), false); break;
    }

    oled_set_cursor(0, 7);
    oled_write_P(PSTR("WPM  "), false);
    oled_set_cursor(0, 8);
    char wpm_buf[6];
    snprintf(wpm_buf, sizeof(wpm_buf), "%3u  ", wpm);
    oled_write(wpm_buf, false);
}

static void render_slave(void) {
    if (!is_oled_on()) return;
    for (uint8_t row = 0; row < 32; row++) {
        for (uint8_t col_byte = 0; col_byte < 4; col_byte++) {
            uint8_t byte = pgm_read_byte(&bulldog_pixels[row * 4 + col_byte]);
            for (uint8_t bit = 0; bit < 8; bit++) {
                uint8_t x = col_byte * 8 + bit;
                bool on = (byte & (1 << (7 - bit))) != 0;
                oled_write_pixel(x, row, on);
            }
        }
    }
    oled_set_cursor(0, 5);
    oled_write_P(PSTR("Up\n"), false);
    uint32_t uptime_s = timer_read32() / 1000;
    uint32_t h = uptime_s / 3600;
    uint8_t m = (uptime_s / 60) % 60;
    uint8_t s = uptime_s % 60;
    char buf[8];
    if (h > 0) {
        snprintf(buf, sizeof(buf), "%2lu:%02u", (unsigned long)h, m);
    } else {
        snprintf(buf, sizeof(buf), "%02u:%02u", m, s);
    }
    oled_write(buf, false);
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        render_master();
    } else {
        render_slave();
    }
    return false;
}

#endif

#ifdef OTHER_KEYMAP_C
#    include OTHER_KEYMAP_C
#endif
