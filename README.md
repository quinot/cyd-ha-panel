CYD HA Panel
============

ESPHome firmware for a Home Assistant wall panel based on the **ESP32-2432S032C**
("Cheap Yellow Display" v2, capacitive touch variant).

Controls 5 roof windows, 5 associated blinds, and HA scenes via LVGL touch UI.

---

## Hardware

Tested on the **ESP32-2432S032C** (Cheap Yellow Display v2, capacitive touch variant - [AliExpress](https://s.click.aliexpress.com/e/_c3fBS1d7)).

| Component | Details |
|-----------|---------|
| MCU | ESP32-WROOM-32 · `esp32dev` board · 4 MB flash · **DIO flash mode** (QIO causes boot loops on this board) · no PSRAM |
| Display | ST7789V 320×240 IPS TFT via SPI · MOSI=GPIO13 · CLK=GPIO14 · CS=GPIO15 · DC=GPIO2 · no RST pin · `invert_colors: true` required |
| Display transform | `mirror_x: false` · `mirror_y: true` · `swap_xy: true` (do **not** use `rotation:` — it is mutually exclusive with `transform:`) |
| Backlight | GPIO27 · LEDC PWM · active-high |
| Touch | GT911 capacitive · I2C · SDA=GPIO33 · SCL=GPIO32 · RST=GPIO25 · address `0x5D` · **no INT pin** — driver runs in polling mode (omit `interrupt_pin:` entirely) |
| Touch transform | `mirror_x: true` · `mirror_y: false` · `swap_xy: true` |
| RGB status LED | Active-low (inverted) · R=GPIO4 · G=GPIO16 · B=GPIO17 |
| Light sensor | GT36516 photoresistor on GPIO34 (ADC1_CH6, input-only) |

> **GT911 I2C pin variance** — some 2432S032C boards use SDA=GPIO21 / SCL=GPIO22
> instead of GPIO33/GPIO32. If touch is completely unresponsive after flashing,
> swap these two pins in `packages/hardware.yaml`.

### Ambient light sensor circuit

The GT36516 LDR sits in **parallel** with R19 (1 MΩ) between IO34 and GND,
with R15 (1 MΩ) as the pull-up to 3.3 V:

```
3.3V ── R15 (1MΩ) ── IO34 ──┬── R19 (1MΩ) ── GND
                            └── GT36516   ── GND
```

Voltage range:
- **Bright (≈10 lux):** GT36516 ≈ 5–10 kΩ → parallel ≈ 5–10 kΩ → V_IO34 ≈ 16–32 mV
- **Dark:** GT36516 ≈ 0.3 MΩ → parallel ≈ 231 kΩ → V_IO34 ≈ 0.62 V

Maximum voltage ≈ 0.62 V → use `attenuation: 0db` (0–0.95 V range) for best resolution.
Note: brighter = lower voltage (LDR resistance drops, pulling IO34 toward GND).

### Power supply

The board can take power (5V) from the USB C connector, or from a 4-pin Molex Microblade which also provides access to the serial port.
I'm powering it from a [switching AC power supply module](https://s.click.aliexpress.com/e/_c3ZsVazP) hidden behind it in a wall mount box, on which I soldered a [pigtail connector](https://s.click.aliexpress.com/e/_c2wNZYbb) (not a real MicroBlade, but close enough).

---

## Quick start

```sh
# 1. Fill in credentials
cp secrets.yaml.example secrets.yaml
$EDITOR secrets.yaml

# 2. Edit entity IDs and labels in cyd-ha-panel.yaml (substitutions section)

# 3. In HA: Settings → Devices & Services → ESPHome → Configure →
#    enable "Allow device to perform Home Assistant actions"

# 4. Compile and flash
esphome run cyd-ha-panel.yaml
```

---

## File structure

```
cyd-ha-panel/
├── cyd-ha-panel.yaml          # Top-level: substitutions, wifi, api, ota, packages
├── secrets.yaml               # WiFi/OTA credentials (gitignored)
├── secrets.yaml.example       # Template for secrets.yaml
└── packages/
    ├── hardware.yaml          # SPI, I2C, display, touch, backlight, RGB LED,
    │                          # ambient light sensor, WiFi signal sensor
    ├── entities.yaml          # HA sensor/text_sensor imports (positions, states)
    ├── screensaver.yaml       # Time source, globals, 1s interval (clock + drift)
    ├── ui.yaml                # Colour palette, fonts, LVGL theme, page list
    ├── ui_main.yaml           # Tabview shell (includes the four tab files)
    ├── ui_main_scenes.yaml    # Scènes tab
    ├── ui_main_windows.yaml   # Fenêtres tab
    ├── ui_main_blinds.yaml    # Stores tab
    ├── ui_main_info.yaml      # Info tab (clock, LDR, WiFi)
    ├── ui_screensaver.yaml    # Screensaver page (bouncing clock)
    ├── mock_data.yaml         # Fake sensor data for UI testing without HA
    └── fonts/
        ├── DejaVuSans.ttf                  # Button icons ▲■▼ (Geometric Shapes U+25A0–25FF)
        ├── DejaVuSansMono.ttf              # Signal bars ▂▄▆█ (Block Elements U+2580–259F)
        └── materialdesignicons-webfont.ttf # Scene button MDI icons (Private Use Area)
```

---

## Configurable substitutions

All customisation lives in the `substitutions:` block at the top of
`cyd-ha-panel.yaml`. Nothing under `packages/` needs to change for
routine reconfiguration.

| Key | Default | Description |
|-----|---------|-------------|
| `device_name` | `cyd-ha-panel` | ESPHome device hostname |
| `friendly_name` | `"CYD HA Panel"` | Display name in HA |
| `screensaver_timeout` | `"60"` | Seconds of idle before screensaver activates |
| `screensaver_brightness` | `"0.3"` | Backlight level during screensaver (0.0–1.0) |
| `antiburn_delay` | `"60"` | Seconds of screensaver before pixel-snow can start (gives time to read the clock) |
| `win1_id` … `win5_id` | — | HA `cover.*` entity IDs for the 5 roof windows |
| `win1_name` … `win5_name` | — | Row labels (≤ ~10 chars; `font_small`, 78 px wide) |
| `blind1_id` … `blind5_id` | — | HA `cover.*` entity IDs for the 5 blinds |
| `blind1_name` … `blind5_name` | — | Row labels |
| `scene1_id` … `scene4_id` | — | HA `scene.*` entity IDs |
| `scene1_name` … `scene4_name` | — | Button labels (wrap at ≈ 12 chars in 148×85 px) |
| `scene1_icon` … `scene4_icon` | — | MDI glyph character (e.g. `"\U000F0599"`); set to `"none"` or `""` to show no icon |

---

## UI overview

### Tab layout

Four tabs at the bottom (30 px tab bar, 210 px content area):

| Tab | Content |
|-----|---------|
| **Scènes** | 2×2 grid of scene buttons |
| **Fenêtres** | 5 roof-window cover rows |
| **Stores** | 5 blind cover rows |
| **Info** | Clock, LDR reading, WiFi SSID and signal |

### Cover rows (Fenêtres / Stores)

Each row (40 px tall, 320 px wide):

```
[● dot 12px] [name 78px] [████░ bar 100px] [100% 34px] [▲ 26px] [■ 26px] [▼ 26px]
```

- **Dot colour** reflects HA cover state: open=green · opening=teal ·
  closing=orange · stopped=yellow · closed=grey
- **Bar** shows `current_position` (0–100)
- **▲ / ■ / ▼** call `cover.open_cover` / `cover.stop_cover` / `cover.close_cover`

State dot colours are updated via `lambda` in each `text_sensor`'s `on_value`
callback in `packages/entities.yaml`. There is no generic script — ESPHome LVGL
does not support referencing widget IDs by string at runtime.

### Screensaver

Activates after `${screensaver_timeout}` seconds of no touch input (LVGL
`on_idle`). On activation:

1. The active tab index is saved to `globals.saved_tab`.
2. `screensaver_active` global is set to `true`.
3. Backlight dims to `${screensaver_brightness}` over 1 s.
4. LVGL switches to `page_screensaver` (black background, no tab bar).

A 1-second `interval:` (in `screensaver.yaml`) updates the `HH:MM:SS` and
French date labels. While `screensaver_active` is true, the clock container
drifts 1 px/s in a bouncing-ball pattern — movement bounds are computed at
runtime from `lv_obj_get_width/height(ss_container)` so they stay correct
regardless of font metrics.

On any **touch release**, if `screensaver_active` is true:

1. `screensaver_active` is set to `false`.
2. LVGL restores the previously active tab via `lv_tabview_set_act`.
3. Backlight returns to full brightness over 300 ms.
4. LVGL switches back to `page_main`.

The wake handler lives in `packages/hardware.yaml` inside the GT911
`touchscreen.on_release` block — it cannot be in a separate file because
ESPHome does not support multiple `touchscreen:` platform entries.

The same 1-second interval also updates the clock labels on the **Info tab**
(`info_time`, `info_date`) so both views stay in sync without a second timer.

### Anti-burn-in (nightly pixel snow)

TFT displays suffer from image retention when the same pixels are lit for
extended periods. The screensaver bouncing clock already helps, but dedicated
**pixel snow** exercises every subpixel uniformly.

Anti-burn is a second phase of the screensaver — it can only activate while
the screensaver is already running. The 1-second interval that drives the
clock and the bouncing animation also evaluates two conditions every tick:

1. The screensaver has been continuously active for at least `${antiburn_delay}`
   seconds (default 60 s) — enough time for the user to see the clock.
2. The current time falls within a designated night slot: hours 2–5,
   minutes :05–:34.

When both are true the `switch_antiburn` template switch is turned on:

- **turn_on**: fades the backlight to 0 % over 1 s, then pauses LVGL with
  `show_snow: true` (random-coloured pixels, cycling continuously).
- **turn_off**: resumes LVGL, requests a full redraw, then restores the
  backlight to `${screensaver_brightness}` (screensaver still active) or to
  full brightness (if the device is also waking up).

The snow is invisible to anyone in the room because the backlight is off.

Anti-burn turns off automatically as soon as either condition above becomes
false (slot ends at :35, or screensaver is dismissed). Any **touch release**
while LVGL is paused calls `switch.turn_off: switch_antiburn` immediately
(via the `lvgl.is_paused` guard in the GT911 `on_release` handler), then
falls through to the normal screensaver-wake path.

---

## Fonts

### Why local TTF files?

Block characters (▂▄▆█, U+2580–259F) and geometric shapes (▲■▼, U+25A0–25FF)
are **not reliably available via `gfonts://`**. Google Fonts serves reduced
Unicode subsets — only a handful of glyphs from a font are included, and these
ranges are consistently missing. Confirmed failures:
- `gfonts://Noto+Sans+Symbols+2` — all 4 block chars missing
- `gfonts://Ubuntu+Mono` — only █ (U+2588) present; ▂▄▆ missing

**DejaVu fonts cover the full Block Elements and Geometric Shapes ranges.**

### Font files

**DejaVu** — downloaded from the official release zip:
```
https://github.com/dejavu-fonts/dejavu-fonts/releases/download/version_2_37/dejavu-fonts-ttf-2.37.zip
```
Extract `ttf/DejaVuSans.ttf` and `ttf/DejaVuSansMono.ttf` into `packages/fonts/`.

**Material Design Icons** — downloaded from jsDelivr (npm `@mdi/font`):
```
https://cdn.jsdelivr.net/npm/@mdi/font@7.4.47/fonts/materialdesignicons-webfont.ttf
```
Save as `packages/fonts/materialdesignicons-webfont.ttf`. Codepoints used:

| Icon | MDI name | Codepoint |
|------|----------|-----------|
| ☀ | `weather-sunny` | `\U000F0599` |
| 💨 | `weather-windy` | `\U000F059D` |
| 🎬 | `projector-screen` | `\U000F042F` |

| File | Used in | Glyphs |
|------|---------|--------|
| `DejaVuSans.ttf` | `font_icons` (16 px) | `▲■▼` — cover control buttons |
| `DejaVuSansMono.ttf` | `font_medium` extras (14 px) | `▂▄▆█` — WiFi signal bars |

DejaVu Sans Mono is preferred for the signal bars because its fixed-width
glyphs give consistent bar widths regardless of how many bars are shown.

### Font palette

| ID | File | Size | Used for |
|----|------|------|---------|
| `font_small` | Roboto | 11 px | Cover row name labels |
| `font_medium` | Roboto + DejaVuSansMono extras | 14 px | General labels, tab bar, signal bars |
| `font_scene` | Roboto Bold | 13 px | Scene button labels |
| `font_mdi` | MaterialDesignIcons webfont | 28 px | Scene button icons (sun, wind, screen) |
| `font_icons` | DejaVuSans | 16 px | ▲■▼ cover control buttons |
| `font_clock` | Roboto | 48 px | Screensaver / Info tab HH:MM:SS |
| `font_clock_date` | Roboto | 16 px | Screensaver / Info tab date line |

---

## Extending the project

### Adding a 6th cover

1. Add `win6_id` / `win6_name` substitutions in `cyd-ha-panel.yaml`.
2. Add a `sensor` + `text_sensor` block in `packages/entities.yaml`.
3. Add a `win6_row` object in `packages/ui_main_windows.yaml`.
4. Reduce all row heights from 40 px to 33 px (6 × 33 + 5 top pad = 203 px,
   fits within the 210 px content area).

### Adding a 4th scene

Scenes currently fill a 2×2 grid. A 5th/6th button can be added by extending
the flex layout in `packages/ui_main_scenes.yaml` — the container will scroll
or you can switch to a 2×3 grid (3 rows × 1 column each of 2 buttons).

### Disabling HA during UI development

Uncomment the `mock:` line in `cyd-ha-panel.yaml`:

```yaml
mock: !include packages/mock_data.yaml
```

This injects fake position/state values so the UI renders correctly without
a live HA connection.

---

## Colour palette

| Token | Hex | Use |
|-------|-----|-----|
| `col_bg` | `#1a1a2e` | Screen background (dark navy) |
| `col_card` | `#16213e` | Alternating row tint |
| `col_accent` | `#00b4d8` | Progress bar fill, scene button border |
| `col_text` | `#ffffff` | Primary labels |
| `col_text_dim` | `#9e9e9e` | Secondary labels (LDR, WiFi) |
| `col_bar_bg` | `#1e4a7a` | Progress bar track |
| `col_btn_open` | `#1b5e20` | ▲ Open button |
| `col_btn_stop` | `#e65100` | ■ Stop button |
| `col_btn_close` | `#7f0000` | ▼ Close button |
| `col_scene_btn` | `#0d3b5e` | Scene card background |
| `col_tab_active` | `#00b4d8` | Active tab indicator |
| `col_tab_bar` | `#0a0a1a` | Tab bar background |
| `col_black` | `#000000` | Screensaver background |
| `col_ss_time` | `#e0e0e0` | Screensaver / Info clock digits |
| `col_ss_date` | `#c0c0c0` | Screensaver / Info date line |
| `col_state_closed` | `#555555` | Cover-state dot: closed/unknown |
