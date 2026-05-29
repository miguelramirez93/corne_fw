# corne_fw — Corne RP2040 firmware (`static_ug` keymap)

Custom QMK firmware for a Corne (`crkbd/rev1`) split keyboard running on Elite-Pi (RP2040) controllers, converted from the original AVR build via `CONVERT_TO=rp2040_ce`.

## Features

- **4 layers**: BASE / NAV / SYM / FN
- **Per-layer underglow color** (BASE off, NAV cyan, SYM purple, FN red)
- **Underglow only**: per-key LEDs disabled, only the 6 underglow LEDs per side
- **Master OLED**: animated dog (WPM-reactive), current layer, Caps Lock, active modifiers
- **Slave OLED**: static bulldog face
- **Cross-OS screenshot key** (`SS_SEL` on layer 3) — detects host OS and sends the right shortcut
- **Media keys** on layer 3: Mute / Prev / Play-Pause / Next / Vol-/Vol+

## Repository layout

```
corne_fw/
├── README.md
├── static_ug/                  # QMK keymap (drop into qmk_firmware)
│   ├── keymap.c
│   ├── config.h
│   └── rules.mk
└── tools/                      # Image conversion helpers
    ├── corne_logo.png          # Source bulldog image
    ├── png2oled.py             # PNG → SSD1306 byte array (centered)
    ├── png2oled_top.py         # PNG → byte array (top-placed)
    ├── png2oled_top_strict.py  # Same, with stricter threshold + autocontrast
    ├── inspect_dog.py          # Render byte array as ASCII art
    ├── render_logo.py          # Render byte array to PNG preview
    ├── make_frame_b.py         # Generate an upward-shifted animation frame
    └── wag_tail.py             # Generate tail-wag animation frame
```

## Prerequisites

macOS (Homebrew):

```bash
brew install qmk/qmk/qmk
brew install python@3.13                 # qmk virtualenv interpreter
brew tap osx-cross/arm && brew tap osx-cross/avr
brew install arm-none-eabi-binutils
brew install dos2unix
```

The qmk formula has dependencies on `python@3.13`, `arm-none-eabi-gcc@8`, and `arm-none-eabi-binutils`. After installation, two of those (binutils, gcc) end up keg-only — you must extend PATH:

```bash
export PATH="/opt/homebrew/opt/arm-none-eabi-gcc@8/bin:/opt/homebrew/opt/arm-none-eabi-binutils/bin:$PATH"
```

## Setup

1. Clone the upstream QMK firmware tree (one-time):

   ```bash
   qmk setup -y     # clones to ~/qmk_firmware
   ```

2. Drop the keymap into QMK:

   ```bash
   cp -r static_ug ~/qmk_firmware/keyboards/crkbd/keymaps/static_ug
   ```

## Build

```bash
export PATH="/opt/homebrew/opt/arm-none-eabi-gcc@8/bin:/opt/homebrew/opt/arm-none-eabi-binutils/bin:$PATH"
cd ~/qmk_firmware
qmk compile -kb crkbd/rev1 -km static_ug -e CONVERT_TO=rp2040_ce
```

Output: `~/qmk_firmware/crkbd_rev1_static_ug_elite_pi.uf2`

## Flash

Each half is flashed independently.

1. Disconnect the TRRS cable between halves.
2. **Double-tap the reset button** on the controller (or short the boot pads, on bare Elite-Pi). The half mounts as a USB drive named `RPI-RP2`.
3. **Drag the `.uf2`** onto the mounted drive. The drive ejects automatically and the controller reboots into the new firmware.
4. Repeat for the other half.
5. Reconnect the TRRS cable. Plug USB into either half.

⚠ Power off (unplug USB) before reconnecting TRRS — hot-plugging TRRS can short contacts during insertion and damage controllers.

## Keymap diagrams

Symbol legend in the diagrams:
- `____` = transparent (falls through to lower active layer)
- `XXXX` = no-op (KC_NO)
- Inner thumb keys: `MO(1)` = hold for NAV, `MO(2)` = hold for SYM; holding both activates FN via the `MO(3)` chord.

### Layer 0 — BASE (underglow off)

```
┌────┬────┬────┬────┬────┬────┐   ┌────┬────┬────┬────┬────┬────┐
│TAB │ Q  │ W  │ E  │ R  │ T  │   │ Y  │ U  │ I  │ O  │ P  │BSPC│
├────┼────┼────┼────┼────┼────┤   ├────┼────┼────┼────┼────┼────┤
│CTL │ A  │ S  │ D  │ F  │ G  │   │ H  │ J  │ K  │ L  │ ;  │ '  │
├────┼────┼────┼────┼────┼────┤   ├────┼────┼────┼────┼────┼────┤
│SFT │ Z  │ X  │ C  │ V  │ B  │   │ N  │ M  │ ,  │ .  │ /  │ESC │
└────┴────┴────┼────┼────┼────┤   ├────┼────┼────┼────┴────┴────┘
               │LGUI│MO 1│SPC │   │ENT │MO 2│RALT│
               └────┴────┴────┘   └────┴────┴────┘
```

### Layer 1 — NAV (cyan underglow)

Hold left inner thumb.

```
┌────┬────┬────┬────┬────┬────┐   ┌────┬────┬────┬────┬────┬────┐
│TAB │ 1  │ 2  │ 3  │ 4  │ 5  │   │ 6  │ 7  │ 8  │ 9  │ 0  │BSPC│
├────┼────┼────┼────┼────┼────┤   ├────┼────┼────┼────┼────┼────┤
│CTL │XXXX│XXXX│XXXX│XXXX│XXXX│   │ ←  │ ↓  │ ↑  │ →  │XXXX│XXXX│
├────┼────┼────┼────┼────┼────┤   ├────┼────┼────┼────┼────┼────┤
│SFT │XXXX│XXXX│XXXX│XXXX│XXXX│   │XXXX│XXXX│XXXX│XXXX│XXXX│XXXX│
└────┴────┴────┼────┼────┼────┤   ├────┼────┼────┼────┴────┴────┘
               │LGUI│ ▲  │SPC │   │ENT │MO 3│RALT│
               └────┴────┴────┘   └────┴────┴────┘
```

### Layer 2 — SYM (purple underglow)

Hold right inner thumb.

```
┌────┬────┬────┬────┬────┬────┐   ┌────┬────┬────┬────┬────┬────┐
│TAB │ !  │ @  │ #  │ $  │ %  │   │ ^  │ &  │ *  │ (  │ )  │BSPC│
├────┼────┼────┼────┼────┼────┤   ├────┼────┼────┼────┼────┼────┤
│CTL │XXXX│XXXX│XXXX│XXXX│XXXX│   │ -  │ =  │ [  │ ]  │ \  │ `  │
├────┼────┼────┼────┼────┼────┤   ├────┼────┼────┼────┼────┼────┤
│SFT │XXXX│XXXX│XXXX│XXXX│XXXX│   │ _  │ +  │ {  │ }  │ |  │ ~  │
└────┴────┴────┼────┼────┼────┤   ├────┼────┼────┼────┴────┴────┘
               │LGUI│MO 3│SPC │   │ENT │ ▲  │RALT│
               └────┴────┴────┘   └────┴────┴────┘
```

### Layer 3 — FN (red underglow)

Hold both inner thumbs (tri-layer chord).

```
┌────┬────┬────┬────┬────┬────┐   ┌────┬────┬────┬────┬────┬────┐
│ F1 │ F2 │ F3 │ F4 │ F5 │ F6 │   │ F7 │ F8 │ F9 │F10 │F11 │F12 │
├────┼────┼────┼────┼────┼────┤   ├────┼────┼────┼────┼────┼────┤
│XXXX│XXXX│SHOT│XXXX│XXXX│XXXX│   │MUTE│PREV│PLAY│NEXT│XXXX│XXXX│
├────┼────┼────┼────┼────┼────┤   ├────┼────┼────┼────┼────┼────┤
│XXXX│XXXX│XXXX│XXXX│XXXX│BOOT│   │XXXX│VOL-│VOL+│XXXX│XXXX│XXXX│
└────┴────┴────┼────┼────┼────┤   ├────┼────┼────┼────┴────┴────┘
               │LGUI│ ▲  │SPC │   │ENT │ ▲  │RALT│
               └────┴────┴────┘   └────┴────┴────┘
```

- Top row: `F1`–`F12`.
- `BOOT` = `QK_BOOT` (enter bootloader, no physical reset needed). Located at the **B position** (left half, bottom row, inner-most key).
- `SHOT` = `SS_SEL` (OS-aware selection screenshot):
  - macOS / iOS / unknown → ⌘⇧4 (screenshot selection → file on Desktop)
  - Windows → ⊞+⇧+S (Snipping Tool selection → clipboard)
  - Linux → ⇧+PrintScreen (selection in GNOME/KDE/etc.)

## OLEDs

### Master (the half with USB)

```
┌─────┐
│ 🐕   │  ← Animated dog. Faster wag at higher WPM.
│ 🐕   │      WPM 0     → swap every 600 ms
│ 🐕   │      WPM 1–29  → 350 ms
│     │      WPM 30+   → 150 ms
│Layer│
│NAV  │  ← BASE / NAV / SYM / FN
│     │
│Caps │
│ON   │  ← ON when Caps Lock engaged, OFF otherwise
│     │
│Mod  │
│-C-G │  ← S C A G for Shift / Ctrl / Alt / Gui (held = letter, off = -)
└─────┘
```

### Slave

```
┌─────┐
│ 🐶  │  ← Static bulldog face
│ 🐶  │
│ 🐶  │
│ 🐶  │
│     │
└─────┘
```

Mods and host-OS were intentionally **not** displayed on the slave: enabling any extra split-transport sync (`SPLIT_WPM_ENABLE`, custom `SPLIT_TRANSACTION_IDS_USER`) caused the slave to drop off intermittently on this RP2040 + Elite-Pi + QMK combo. The lean-sync configuration here is stable.

## RGB / underglow config

- `RGBLIGHT_ENABLE = yes` (subsystem compiled in, but **not used at runtime** — see below), `RGB_MATRIX_ENABLE = no`
- 6 underglow LEDs per side (`RGBLIGHT_LED_COUNT 6`, no `RGBLIGHT_SPLIT_COUNT`, no `RGBLIGHT_SPLIT`)
- Brightness capped at `RGBLIGHT_LIMIT_VAL 180` (~70%) — vestigial, doesn't affect anything now that we bypass RGBLIGHT
- **Each half drives its own LEDs via direct `ws2812_set_color_all()` + `ws2812_flush()` calls.** The RGBLIGHT subsystem is bypassed entirely.

### Why direct WS2812 instead of RGBLIGHT

We went through three iterations before landing here:

1. **First attempt — `RGBLIGHT_SPLIT` ON.** Master pushes RGB state to slave via the split transport. Failed: sync packets are unreliable on this RP2040 + RP2040 serial half-duplex transport. Slave stays at boot-default (off) and only flickers to the right color when master triggers a layer change, then drops back.

2. **Second attempt — `RGBLIGHT_SPLIT` OFF, slave calls `rgblight_sethsv_noeeprom` locally from `housekeeping_task_user`.** Each half independently runs the same RGBLIGHT update code on the synced layer state. Failed too: confirmed via OLED diagnostic that `layer_state` syncs correctly to the slave, but the slave's `rgblight_sethsv_noeeprom` call doesn't actually produce LED output. The RGBLIGHT subsystem on this build appears to have some master-only guard somewhere in its hot path, even with `RGBLIGHT_SPLIT` undefined.

3. **Final solution — bypass RGBLIGHT.** From `housekeeping_task_user`, when the synced layer changes, call `ws2812_set_color_all(r, g, b)` + `ws2812_flush()` directly. This is QMK's lowest-level LED driver API and it has no concept of master/slave. Both halves drive their local WS2812 chains the same way. Works perfectly.

### Code

```c
static void force_leds(uint8_t r, uint8_t g, uint8_t b) {
    ws2812_set_color_all(r, g, b);
    ws2812_flush();
}

void housekeeping_task_user(void) {
    static uint8_t last_layer = 0xFF;
    uint8_t layer = get_highest_layer(layer_state);
    if (layer != last_layer) {
        last_layer = layer;
        switch (layer) {
            case 1: force_leds(0, 80, 80); break;   // NAV → cyan
            case 2: force_leds(60, 0, 80); break;   // SYM → purple
            case 3: force_leds(80, 0, 0); break;    // FN  → red
            default: force_leds(0, 0, 0); break;    // BASE → off
        }
    }
}
```

Colors are raw RGB values in QMK's WS2812 driver order (G, R, B is what the WS2812 chip wants on the wire, but the `ws2812_set_color_all` helper takes them in R, G, B order). Adjust intensities by tweaking the numbers.

## Image conversion (OLED art)

There are **two different image encodings** in this firmware, picked based on which OLED API the renderer uses. They are not interchangeable — picking the wrong one is the most common cause of "image is broken/garbled" on Corne + RP2040.

### Encoding A — SSD1306 page-byte (for `oled_write_raw_P`)

- Used on **master** for the animated dog frames.
- 512 bytes total = 4 pages × 128 cols, each byte = 8 vertical pixels, LSB = top.
- Bytes go directly into the SSD1306 framebuffer **without QMK's rotation transform** (see "Known oddity" below).
- Tools:
  - `tools/png2oled.py` — image centered in 32×128 portrait
  - `tools/png2oled_top.py` — image placed at top
  - `tools/png2oled_top_strict.py` — top placement + autocontrast + adjustable threshold (strips watermarks)
  - `tools/preview_bulldog.py` — decodes a byte array back to PNGs to sanity-check before flashing
  - `tools/inspect_dog.py` — ASCII art preview of a byte array
  - `tools/render_logo.py` — PNG preview of the `raw_logo` dog
  - `tools/make_frame_b.py`, `tools/wag_tail.py` — generate animation frames from a base frame

### Encoding B — row-major 32×32 bitmap (for `oled_write_pixel`)

- Used on **slave** for the bulldog face.
- 128 bytes total = 32 rows × 4 bytes (MSB-first within each byte).
- Rendered pixel-by-pixel through `oled_write_pixel(x, y, on)` so QMK's rotation transform IS applied. Slower (1024 calls per frame) but always orientation-correct.
- Tool:
  - `tools/png2pixels.py` — converts PNG to a `static const uint8_t PROGMEM bulldog_pixels[128]` array

Regenerate the bulldog from the included PNG:

```bash
python3 tools/png2pixels.py tools/corne_logo.png 32 80
```

Arguments: `<source-png> <target-height> <black-threshold>`. Paste the output into `keymap.c` replacing the existing `bulldog_pixels[]`.

### Known oddity — `oled_write_raw_P` does NOT honor rotation here

On this combination (QMK + RP2040 + Elite-Pi converter + crkbd/rev1 + SSD1306 OLED), `oled_write_raw_P` writes bytes directly to the framebuffer in **native landscape layout (128×32)**, *ignoring* the `OLED_ROTATION_270` setting. Bytes appear on the display as if rotation were `OLED_ROTATION_0`.

If you've prepared bytes for the rotated portrait orientation (32×128) and use `oled_write_raw_P`, the image comes out garbled, squashed, or with gaps. **Two workarounds**:

1. **Use `oled_write_pixel` per pixel** (what the slave bulldog does). Goes through the rotation transform, always correct, slightly slower.
2. **Pre-encode the bytes to match the native landscape layout** that the buffer expects (what the master dog frames effectively do — those bytes happen to land in the right place because they were short, top-of-buffer writes that visually align with "top of portrait").

For new art on the slave OLED, prefer approach 1. For the dog animation we keep approach 2 because the byte arrays were already known-good and re-encoding them per-pixel would balloon the keymap size unnecessarily for a 4-frame loop.

## Caveats / known issues

- **TRRS cable matters.** Long, thin, or unshielded cables cause symptoms like "slave LEDs dark while everything else works" (power sag) or "slave silently drops off" (signal integrity). Prefer short (< 50 cm) shielded TRRS cables sold for split keyboards.
- **Avoid extra split sync** on this hardware. `SPLIT_WPM_ENABLE` and custom `SPLIT_TRANSACTION_IDS_USER` both reproducibly cause slave disconnects. Anything you want to show on the slave OLED must be computed locally on the slave or be a static asset.
- **RGBLIGHT subsystem is bypassed entirely**. Don't call `rgblight_*` functions — they silently no-op on the slave half in this build for reasons we couldn't pin down. Use `ws2812_set_color_all()` + `ws2812_flush()` instead. See "RGB / underglow config" for the working pattern.
- **OS detection delay**: `detected_host_os()` returns `OS_UNSURE` for ~2 s after plugging in. The screenshot key defaults to macOS during that window.
- **`oled_write_raw_P` ignores rotation.** See "Known oddity" in the image-conversion section. Use `oled_write_pixel` for any new art that needs portrait orientation.

## License

MIT for the keymap source and conversion tools. The `corne_logo.png` is the user-provided source image — verify licensing before redistributing.
