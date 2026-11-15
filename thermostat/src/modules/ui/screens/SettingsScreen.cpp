#include "modules/ui/screens/SettingsScreen.h"
#include "modules/ui/ui_events.h"
#include <iomanip>
#include <sstream>
#include <string>

static void onBackButtonClicked(lv_event_t*) {
    postUIEvent({UIEventId::SwitchScreen, static_cast<int>(ScreenId::Home)});
}

void SettingsScreen::create(lv_obj_t* root) {
    container = root;

    lv_obj_t* content = lv_obj_create(container);
    lv_obj_set_size(content, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(content, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(content, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_all(content, 10, 0);

    iaqLabel = lv_label_create(content);
    lv_obj_set_style_text_color(iaqLabel, lv_color_hex(COLOR_TEXT_MAIN), 0);
    lv_obj_set_style_text_font(iaqLabel, &lv_font_montserrat_24, 0);

    backButton = lv_button_create(content);
    lv_obj_t* btnLabel = lv_label_create(backButton);
    lv_label_set_text(btnLabel, "Back");
    lv_obj_add_event_cb(backButton, onBackButtonClicked, LV_EVENT_CLICKED, nullptr);

    created = true;
}

void SettingsScreen::show() {
    lv_obj_clear_flag(container, LV_OBJ_FLAG_HIDDEN);
}

void SettingsScreen::hide() {
    lv_obj_add_flag(container, LV_OBJ_FLAG_HIDDEN);
}

void SettingsScreen::update(const AppState& appState) {
    lv_label_set_text(iaqLabel, ("IAQ: " + std::to_string(appState.currIAQ)).c_str());
}
