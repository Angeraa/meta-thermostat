#include "modules/ui/screens/HomeScreen.h"
#include "modules/ui/ui_events.h"
#include "lvgl/lvgl.h"
#include "utils/formatting.h"

extern "C" {
extern const lv_font_t ui_font_montserrat_240;
}

void heatSetpointCallback(void* context, int newSetpoint) {
    postUIEvent({UIEventId::HeatSetpointChanged, newSetpoint});
}

void coolSetpointCallback(void* context, int newSetpoint) {
    postUIEvent({UIEventId::CoolSetpointChanged, newSetpoint});
}

static void onmenuButtonClicked(lv_event_t*) {
    postUIEvent({UIEventId::SwitchScreen, static_cast<int>(ScreenId::Settings)});
}

static void onhvacModeButtonClicked(lv_event_t*) {
    // Will actually switch to a specific screen to pick the HVAC state, but for now just switch to the settings screen
    postUIEvent({UIEventId::SwitchScreen, static_cast<int>(ScreenId::Settings)});
}

static void onheatSetpointButtonClicked(lv_event_t* e) {
    HomeScreen* screen = static_cast<HomeScreen*>(lv_event_get_user_data(e));
    postUIEvent({
        UIEventId::OpenSetpointDialog,
        screen->getHeatSetpoint(),
        heatSetpointCallback,
        screen
    });
}

static void oncoolSetpointButtonClicked(lv_event_t* e) {
    HomeScreen* screen = static_cast<HomeScreen*>(lv_event_get_user_data(e));
    postUIEvent({
        UIEventId::OpenSetpointDialog,
        screen->getCoolSetpoint(),
        coolSetpointCallback,
        screen
    });
}

void HomeScreen::create(lv_obj_t* root) {
    container = root;

    lv_obj_t* content = lv_obj_create(container);
    lv_obj_set_size(content, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(content, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(content, 0, 0);
    lv_obj_set_style_radius(content, 0, 0);
    lv_obj_remove_flag(content, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(content, 15, 0);

    lv_obj_t* topbar = lv_obj_create(content);
    lv_obj_set_size(topbar, LV_PCT(100), 75);
    lv_obj_set_style_pad_all(topbar, 0, 0);
    lv_obj_set_style_bg_opa(topbar, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(topbar, 0, 0);
    lv_obj_set_style_radius(topbar, 0, 0);
    lv_obj_remove_flag(topbar, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(topbar, LV_ALIGN_TOP_MID, 0, 0);

    menuButton = lv_button_create(topbar);
    lv_obj_set_size(menuButton, 100, 50);
    lv_obj_set_style_bg_opa(menuButton, LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_width(menuButton, 0, 0);
    lv_obj_align(menuButton, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_event_cb(menuButton, onmenuButtonClicked, LV_EVENT_CLICKED, nullptr);

    lv_obj_t* menuLabel = lv_label_create(menuButton);
    lv_obj_set_style_text_color(menuLabel, lv_color_hex(COLOR_TEXT_MAIN), 0);
    lv_obj_set_style_text_font(menuLabel, &lv_font_montserrat_24, 0);
    lv_obj_center(menuLabel);
    lv_label_set_text_static(menuLabel, "Menu");

    hvacModeButton = lv_button_create(topbar);
    lv_obj_set_size(hvacModeButton, 100, 50);
    lv_obj_set_style_bg_opa(hvacModeButton, LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_width(hvacModeButton, 0, 0);
    lv_obj_align(hvacModeButton, LV_ALIGN_CENTER, 0, 0);

    modeLabel = lv_label_create(hvacModeButton);
    lv_obj_set_style_text_color(modeLabel, lv_color_hex(COLOR_TEXT_MAIN), 0);
    lv_obj_set_style_text_font(modeLabel, &lv_font_montserrat_24, 0);
    lv_label_set_text_static(modeLabel, "AUTO");

    tempLabel = lv_label_create(content);
    lv_obj_set_pos(tempLabel, 0, 535);
    lv_obj_set_size(tempLabel, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_text_color(tempLabel, lv_color_hex(COLOR_TEXT_MAIN), 0);
    lv_obj_set_style_text_font(tempLabel, &ui_font_montserrat_240, 0);
    lv_obj_set_style_text_align(tempLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_static(tempLabel, "23.5");

    lv_obj_t* environmentContainer = lv_obj_create(content);
    lv_obj_set_pos(environmentContainer, 145, 745);
    lv_obj_set_size(environmentContainer, 400, 70);
    lv_obj_set_style_pad_all(environmentContainer, 0, 0);
    lv_obj_set_style_bg_opa(environmentContainer, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(environmentContainer, 0, 0);
    lv_obj_set_style_radius(environmentContainer, 0, 0);
    lv_obj_remove_flag(environmentContainer, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_flex_flow(environmentContainer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(
        environmentContainer,
        LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_SPACE_EVENLY,
        LV_FLEX_ALIGN_CENTER
    );

    humidityLabel = lv_label_create(environmentContainer);
    lv_obj_set_style_text_color(humidityLabel, lv_color_hex(COLOR_TEXT_MAIN), 0);
    lv_obj_set_style_text_font(humidityLabel, &lv_font_montserrat_32, 0);
    lv_label_set_text_static(humidityLabel, "60% r.H.");

    lv_obj_t* environmentSeparator = lv_label_create(environmentContainer);
    lv_obj_set_style_text_color(environmentSeparator, lv_color_hex(COLOR_TEXT_MAIN), 0);
    lv_obj_set_style_text_font(environmentSeparator, &lv_font_montserrat_32, 0);
    lv_label_set_text_static(environmentSeparator, "|");

    pressureLabel = lv_label_create(environmentContainer);
    lv_obj_set_style_text_color(pressureLabel, lv_color_hex(COLOR_TEXT_MAIN), 0);
    lv_obj_set_style_text_font(pressureLabel, &lv_font_montserrat_32, 0);
    lv_label_set_text_static(pressureLabel, "800 kPa");

    lv_obj_t* setpointContainer = lv_obj_create(content);
    lv_obj_set_pos(setpointContainer, 145, 880);
    lv_obj_set_size(setpointContainer, 400, 70);
    lv_obj_set_style_pad_all(setpointContainer, 0, 0);
    lv_obj_set_style_bg_opa(setpointContainer, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(setpointContainer, 0, 0);
    lv_obj_set_style_radius(setpointContainer, 0, 0);
    lv_obj_remove_flag(setpointContainer, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_flex_flow(setpointContainer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(
        setpointContainer,
        LV_FLEX_ALIGN_SPACE_AROUND,
        LV_FLEX_ALIGN_SPACE_EVENLY,
        LV_FLEX_ALIGN_CENTER
    );

    heatSetpointButton = lv_button_create(setpointContainer);
    lv_obj_set_size(heatSetpointButton, 150, 70);
    lv_obj_set_style_bg_opa(heatSetpointButton, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(heatSetpointButton, 3, 0);
    lv_obj_set_style_border_color(heatSetpointButton, lv_color_hex(COLOR_HEATING), 0);
    lv_obj_add_event_cb(heatSetpointButton, onheatSetpointButtonClicked, LV_EVENT_CLICKED, this);

    heatLabel = lv_label_create(heatSetpointButton);
    lv_obj_set_style_text_color(heatLabel, lv_color_hex(COLOR_HEATING), 0);
    lv_obj_set_style_text_font(heatLabel, &lv_font_montserrat_32, 0);
    lv_obj_set_style_align(heatLabel, LV_ALIGN_CENTER, 0);
    lv_obj_set_style_text_align(heatLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_static(heatLabel, "20");

    coolSetpointButton = lv_button_create(setpointContainer);
    lv_obj_set_size(coolSetpointButton, 150, 70);
    lv_obj_set_style_bg_opa(coolSetpointButton, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(coolSetpointButton, 3, 0);
    lv_obj_set_style_border_color(coolSetpointButton, lv_color_hex(COLOR_COOLING), 0);
    lv_obj_add_event_cb(coolSetpointButton, oncoolSetpointButtonClicked, LV_EVENT_CLICKED, this);

    coolLabel = lv_label_create(coolSetpointButton);
    lv_obj_set_style_text_color(coolLabel, lv_color_hex(COLOR_COOLING), 0);
    lv_obj_set_style_text_font(coolLabel, &lv_font_montserrat_32, 0);
    lv_obj_set_style_align(coolLabel, LV_ALIGN_CENTER, 0);
    lv_obj_set_style_text_align(coolLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_static(coolLabel, "24");

    border = lv_obj_create(container);
    lv_obj_set_pos(border, -10, -10);
    lv_obj_set_size(border, 740, 1300);
    lv_obj_set_style_pad_all(border, 0, 0);
    lv_obj_set_style_bg_opa(border, LV_OPA_TRANSP, 0);
    lv_obj_remove_flag(border, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_color(border, lv_color_hex(COLOR_BACKGROUND), 0);
    lv_obj_set_style_border_width(border, 25, 0);
    lv_obj_set_style_radius(border, 35, 0);

    created = true;
}

void HomeScreen::show() {
    lv_obj_clear_flag(container, LV_OBJ_FLAG_HIDDEN);
}

void HomeScreen::hide() {
    lv_obj_add_flag(container, LV_OBJ_FLAG_HIDDEN);
}

void HomeScreen::update(const AppState& appState) {
    heatSetpoint = appState.heatSetpoint;
    coolSetpoint = appState.coolSetpoint;
    lv_label_set_text(tempLabel, formatTemperature(appState.currTemp).c_str());
    lv_label_set_text(humidityLabel, formatHumidity(appState.currHum).c_str());
    lv_label_set_text(pressureLabel, formatPressure(appState.currPressure).c_str());
    lv_label_set_text(heatLabel, formatTemperature(appState.heatSetpoint).c_str());
    lv_label_set_text(coolLabel, formatTemperature(appState.coolSetpoint).c_str());
    if (appState.mode == HvacMode::AUTO) {
        lv_label_set_text(modeLabel, "AUTO");
        lv_obj_remove_flag(heatSetpointButton, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(coolSetpointButton, LV_OBJ_FLAG_HIDDEN);
    } else if (appState.mode == HvacMode::HEAT) {
        lv_label_set_text(modeLabel, "HEAT");
        lv_obj_remove_flag(heatSetpointButton, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(coolSetpointButton, LV_OBJ_FLAG_HIDDEN);
    } else if (appState.mode == HvacMode::COOL) {
        lv_label_set_text(modeLabel, "COOL");
        lv_obj_add_flag(heatSetpointButton, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(coolSetpointButton, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_label_set_text(modeLabel, "OFF");
        lv_obj_add_flag(heatSetpointButton, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(coolSetpointButton, LV_OBJ_FLAG_HIDDEN);
    }
    if (appState.state == HvacState::HEATING) {
        lv_obj_set_style_text_color(
            tempLabel,
            lv_color_hex(COLOR_HEATING),
            0
        );
        lv_obj_set_style_border_color(
            border,
            lv_color_hex(COLOR_HEATING),
            0
        );
    } else if (appState.state == HvacState::COOLING) {
        lv_obj_set_style_text_color(
            tempLabel,
            lv_color_hex(COLOR_COOLING),
            0
        );
        lv_obj_set_style_border_color(
            border,
            lv_color_hex(COLOR_COOLING),
            0
        );
    } else {
        lv_obj_set_style_text_color(
            tempLabel,
            lv_color_hex(COLOR_TEXT_MAIN),
            0
        );
        lv_obj_set_style_border_color(
            border,
            lv_color_hex(COLOR_BACKGROUND),
            0
        );
    }
}
