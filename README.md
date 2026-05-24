# corne_fw — Corne RP2040 firmware (`static_ug` keymap)

Custom QMK firmware for a Corne (`crkbd/rev1`) split keyboard running on Elite-Pi (RP2040) controllers, converted from the original AVR build via `CONVERT_TO=elite_pi`.

## Features

- **4 layers**: BASE / NAV / SYM / MEDIA
- **Per-layer underglow color** (BASE off, NAV cyan, SYM purple, MEDIA red)
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
qmk compile -kb crkbd/rev1 -km static_ug -e CONVERT_TO=elite_pi
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
- Inner thumb keys: `MO(1)` = hold for NAV, `MO(2)` = hold for SYM; holding both activates MEDIA via the `MO(3)` chord.

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

### Layer 3 — MEDIA (red underglow)

Hold both inner thumbs (tri-layer chord).

```
┌────┬────┬────┬────┬────┬────┐   ┌────┬────┬────┬────┬────┬────┐
│BOOT│XXXX│XXXX│XXXX│XXXX│XXXX│   │XXXX│XXXX│XXXX│XXXX│XXXX│XXXX│
├────┼────┼────┼────┼────┼────┤   ├────┼────┼────┼────┼────┼────┤
│XXXX│XXXX│SHOT│XXXX│XXXX│XXXX│   │MUTE│PREV│PLAY│NEXT│XXXX│XXXX│
├────┼────┼────┼────┼────┼────┤   ├────┼────┼────┼────┼────┼────┤
│XXXX│XXXX│XXXX│XXXX│XXXX│XXXX│   │XXXX│VOL-│VOL+│XXXX│XXXX│XXXX│
└────┴────┴────┼────┼────┼────┤   ├────┼────┼────┼────┴────┴────┘
               │LGUI│ ▲  │SPC │   │ENT │ ▲  │RALT│
               └────┴────┴────┘   └────┴────┴────┘
```

- `BOOT` = `QK_BOOT` (enter bootloader, no physical reset needed).
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
│NAV  │  ← BASE / NAV / SYM / MEDIA
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

- `RGBLIGHT_ENABLE = yes`, `RGB_MATRIX_ENABLE = no`
- 12 underglow LEDs total, 6 per side (`RGBLIGHT_LED_COUNT 12`, `RGBLIGHT_SPLIT_COUNT { 6, 6 }`)
- Static color, no animation modes compiled — saves flash and keeps power draw predictable across the TRRS link
- Per-layer color set in `layer_state_set_user` using `_noeeprom` variants (no flash wear)
- Brightness capped at `RGBLIGHT_LIMIT_VAL 180` (~70%) to keep current under TRRS comfort

## Image conversion (slave OLED art)

The `tools/` scripts convert PNGs to the SSD1306 page-byte format that `oled_write_raw_P` expects.

Generate a new bulldog from `corne_logo.png`:

```bash
python3 tools/png2oled_top_strict.py tools/corne_logo.png 32 80
```

Arguments: `<source-png> <target-height> <black-threshold>`. Output is a `static const char PROGMEM bulldog_top[] = { … };` 512-byte array — paste it into `keymap.c` replacing the existing one.

Preview a byte array:

```bash
python3 tools/inspect_dog.py     # ASCII art to stdout
python3 tools/render_logo.py     # writes /tmp/raw_logo_preview.png
```

## Caveats / known issues

- **TRRS cable matters.** Long, thin, or unshielded cables cause symptoms like "slave LEDs dark while everything else works" (power sag) or "slave silently drops off" (signal integrity). Prefer short (< 50 cm) shielded TRRS cables sold for split keyboards.
- **Avoid extra split sync** on this hardware. `SPLIT_WPM_ENABLE` and custom `SPLIT_TRANSACTION_IDS_USER` both reproducibly cause slave disconnects. Anything you want to show on the slave OLED must be computed locally on the slave or be a static asset.
- **OS detection delay**: `detected_host_os()` returns `OS_UNSURE` for ~2 s after plugging in. The screenshot key defaults to macOS during that window.

## License

MIT for the keymap source and conversion tools. The `corne_logo.png` is the user-provided source image — verify licensing before redistributing.
