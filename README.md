CYD HA Panel
============

ESPHome configuration for a Home Assistant wall panel using a Cheap Yellow Display with capacitive touch screen.

(ESP32 2432S032C)

## Hardware configuration

Tested on the **ESP32-2432S032C** (Cheap Yellow Display v2, capacitive touch variant).

| Component | Details |
|-----------|---------|
| MCU | ESP32-WROOM-32 · `esp32dev` board · 4 MB flash · **DIO flash mode** (QIO causes boot loops on this board) · no PSRAM |
| Display | ST7789V 320×240 IPS TFT via SPI · MOSI=GPIO13 · CLK=GPIO14 · CS=GPIO15 · DC=GPIO2 · no RST pin · `invert_colors: true` required |
| Display transform | `mirror_x: false` · `mirror_y: true` · `swap_xy: true` (do **not** use `rotation:` — it is mutually exclusive with `transform:`) |
| Backlight | GPIO27 · LEDC PWM · active-high |
| Touch | GT911 capacitive · I2C · SDA=GPIO33 · SCL=GPIO32 · RST=GPIO25 · address `0x5D` · **no INT pin** — driver runs in polling mode (omit `interrupt_pin:` entirely) |
| Touch transform | `mirror_x: true` · `mirror_y: false` · `swap_xy: true` |
| RGB status LED | Active-low (inverted) · R=GPIO4 · G=GPIO16 · B=GPIO17 |

> **GT911 I2C pin variance** — some 2432S032C boards use SDA=GPIO21 / SCL=GPIO22 instead of GPIO33/GPIO32. If touch is completely unresponsive after flashing, swap these two pins.
