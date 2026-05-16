#pragma once
// scene_buttons.h — programmatic scene button content builder
// Included via esphome: includes: in cyd-ha-panel.yaml.
//
// Called from lvgl: on_boot: in ui.yaml for each scene button.
// Builds icon + text (flex column) or text-only (centered) depending on
// whether icon is an empty string — no static YAML height tricks needed.

inline void scene_button_init(lv_obj_t* btn,
                               const lv_font_t* icon_font,
                               const char* icon,
                               const lv_font_t* text_font,
                               const char* text) {
    // Turn the button itself into a flex-column container
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(btn, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(btn, 4, LV_PART_MAIN);

    // Icon label — only created when icon is non-empty and not the sentinel "none"
    if (icon && icon[0] != '\0' && strcmp(icon, "none") != 0) {
        lv_obj_t* icon_lbl = lv_label_create(btn);
        lv_label_set_text(icon_lbl, icon);
        lv_obj_set_style_text_font(icon_lbl, icon_font, LV_PART_MAIN);
        lv_obj_set_style_text_color(icon_lbl, lv_color_hex(0x00b4d8), LV_PART_MAIN); // col_accent
    }

    // Text label — always present
    lv_obj_t* text_lbl = lv_label_create(btn);
    lv_label_set_text(text_lbl, text);
    lv_obj_set_style_text_font(text_lbl, text_font, LV_PART_MAIN);
    lv_obj_set_style_text_color(text_lbl, lv_color_hex(0xffffff), LV_PART_MAIN); // col_text
    lv_obj_set_width(text_lbl, lv_pct(100));
    lv_obj_set_style_text_align(text_lbl, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
}
