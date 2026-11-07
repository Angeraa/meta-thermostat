#include "modules/ui/screens/HomeScreen.h"
#include "modules/ui/ui_events.h"
#include <iomanip>
#include <sstream>
#include <string>

static void onSettingsButtonClicked(lv_event_t*) {
    postUIEvent({UIEventId::SwitchScreen, static_cast<int>(ScreenId::Settings)});
}

static std::string formatTemperature(int tenthsOfDegree) {
    std::ostringstream value;
    value << std::fixed << std::setprecision(1)
          << (static_cast<float>(tenthsOfDegree) / 10.0f);
    return value.str() + " C";
}

void HomeScreen::create(lv_obj_t* root) {
    container = lv_obj_create(root);
    lv_obj_set_size(container, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_all(container, 10, 0);

    tempLabel = lv_label_create(container);
    modeLabel = lv_label_create(container);
    heatSetpointLabel = lv_label_create(container);
    coolSetpointLabel = lv_label_create(container);

    settingsButton = lv_button_create(container);
    lv_obj_t* btnLabel = lv_label_create(settingsButton);
    lv_label_set_text(btnLabel, "Settings");
    lv_obj_add_event_cb(settingsButton, onSettingsButtonClicked, LV_EVENT_CLICKED, nullptr);

    created = true;
}

void HomeScreen::show() {
    lv_obj_clear_flag(container, LV_OBJ_FLAG_HIDDEN);
}

void HomeScreen::hide() {
    lv_obj_add_flag(container, LV_OBJ_FLAG_HIDDEN);
}

void HomeScreen::update(const AppState& appState) {
    lv_label_set_text(tempLabel, ("Temp: " + formatTemperature(appState.currTemp)).c_str());
    lv_label_set_text(modeLabel, ("Mode: " + std::to_string(appState.mode)).c_str());
    lv_label_set_text(heatSetpointLabel, ("Heat SP: " + formatTemperature(appState.heatSetpoint)).c_str());
    lv_label_set_text(coolSetpointLabel, ("Cool SP: " + formatTemperature(appState.coolSetpoint)).c_str());
}
