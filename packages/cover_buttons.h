#pragma once
// cover_buttons.h — shared helpers for cover row button logic
// Included via esphome: includes: in cyd-ha-panel.yaml.
// LVGL headers are already in scope when lambdas call these functions.

// Enable/disable the Open and Close buttons based on current position.
// Call from each cover position sensor's on_value lambda.
inline void cover_update_buttons(lv_obj_t* btn_open, lv_obj_t* btn_close, float pos) {
    if (pos >= 100.0f) lv_obj_add_state(btn_open,   LV_STATE_DISABLED);
    else               lv_obj_clear_state(btn_open,  LV_STATE_DISABLED);
    if (pos <= 0.0f)   lv_obj_add_state(btn_close,  LV_STATE_DISABLED);
    else               lv_obj_clear_state(btn_close, LV_STATE_DISABLED);
}
