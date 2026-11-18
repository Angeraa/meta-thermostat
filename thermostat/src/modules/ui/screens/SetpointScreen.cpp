#include "modules/ui/screens/SetpointScreen.h"
#include "modules/ui/ui_events.h"
#include "lvgl/lvgl.h"
#include "utils/formatting.h"

void onBackClick(lv_event_t *e) {
    SetpointScreen* screen = static_cast<SetpointScreen*>(lv_event_get_user_data(e));
    postUIEvent({UIEventId::SetpointSelected, screen->currentSetpoint});
}

void SetpointScreen::create(lv_obj_t *root) {
    container = root;
    lv_obj_add_flag(container, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(container, onBackClick, LV_EVENT_CLICKED, this);

    setpointArcSlider = lv_arc_create(container);
    lv_obj_set_size(setpointArcSlider, 550, 550);
    lv_obj_align(setpointArcSlider, LV_ALIGN_CENTER, 0, 100);
    lv_arc_set_range(setpointArcSlider, minSetpoint, maxSetpoint);
    lv_arc_set_value(setpointArcSlider, currentSetpoint);
    lv_arc_set_bg_angles(setpointArcSlider, 135, 45);
    lv_obj_set_style_arc_width(setpointArcSlider, 20, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(setpointArcSlider, lv_color_hex(COLOR_HEATING), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(setpointArcSlider, 20, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(setpointArcSlider, lv_color_hex(COLOR_COOLING), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(setpointArcSlider, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(setpointArcSlider, lv_color_hex(COLOR_COOLING), LV_PART_MAIN | LV_STATE_DEFAULT);

    setpointLabel = lv_label_create(container);
    lv_obj_set_style_text_font(setpointLabel, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(setpointLabel, lv_color_hex(COLOR_TEXT_MAIN), 0);
    lv_obj_align(setpointLabel, LV_ALIGN_CENTER, 0, 50);

    created = true;
}

void SetpointScreen::show() {
    lv_obj_clear_flag(container, LV_OBJ_FLAG_HIDDEN);
}

void SetpointScreen::hide() {
    lv_obj_add_flag(container, LV_OBJ_FLAG_HIDDEN);
}

void SetpointScreen::update(const AppState& appState) {
    if (setpointArcSlider) {
        int value = lv_arc_get_value(setpointArcSlider);
        if (value != currentSetpoint) {
            currentSetpoint = value;
            lv_arc_set_value(setpointArcSlider, currentSetpoint);
            lv_label_set_text(setpointLabel, formatTemperature(currentSetpoint).c_str());
        }
    }
}

void SetpointScreen::configure(int setpoint, int min, int max) {
    currentSetpoint = setpoint;
    minSetpoint = min;
    maxSetpoint = max;
    lv_arc_set_range(setpointArcSlider, minSetpoint, maxSetpoint);
    lv_arc_set_value(setpointArcSlider, currentSetpoint);
    lv_label_set_text(setpointLabel, formatTemperature(currentSetpoint).c_str());
}