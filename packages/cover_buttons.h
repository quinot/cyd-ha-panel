#pragma once
// cover_buttons.h — shared LVGL helpers
// Included via esphome: includes: in cyd-ha-panel.yaml.
// LVGL headers and std::string are already in scope when lambdas call these.

// Update the combined WiFi status label.
// Call from both wifi_ssid and wifi_ip on_value callbacks.
// "WiFi: unconnected" when either value is empty; "WiFi: <ssid> / <ip>" otherwise.
inline void update_wifi_label(lv_obj_t* label,
                               const std::string& ssid,
                               const std::string& ip) {
    char buf[96];
    if (ssid.empty() || ip.empty())
        snprintf(buf, sizeof(buf), "WiFi: unconnected");
    else
        snprintf(buf, sizeof(buf), "WiFi: %s / %s", ssid.c_str(), ip.c_str());
    lv_label_set_text(label, buf);
}

// Enable/disable the Open and Close buttons based on current position.
// Call from each cover position sensor's on_value lambda.
inline void cover_update_buttons(lv_obj_t* btn_open, lv_obj_t* btn_close, float pos) {
    if (pos >= 100.0f) lv_obj_add_state(btn_open,   LV_STATE_DISABLED);
    else               lv_obj_clear_state(btn_open,  LV_STATE_DISABLED);
    if (pos <= 0.0f)   lv_obj_add_state(btn_close,  LV_STATE_DISABLED);
    else               lv_obj_clear_state(btn_close, LV_STATE_DISABLED);
}
