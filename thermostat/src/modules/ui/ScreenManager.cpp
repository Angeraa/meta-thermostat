#include "modules/ui/ScreenManager.h"
#include "modules/ui/screenRegistry.h"
#include "utils/logging.h"

void ScreenManager::init() {

    root = lv_obj_create(NULL);
    lv_obj_set_size(root, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_set_style_bg_color(root, lv_color_hex(COLOR_BACKGROUND), 0);
    lv_obj_set_style_border_width(root, 0, 0);
    lv_obj_set_style_radius(root, 0, 0);
    lv_obj_set_style_pad_all(root, 0, 0);
    lv_obj_remove_flag(root, LV_OBJ_FLAG_SCROLLABLE);
    logDebug("Creating screens...", "UI");
    for (auto i = 0; i < screenCount; i++) {
        logDebug("Creating screen: " + std::to_string(i), "UI");
        lv_obj_t* screenRoot = lv_obj_create(root);
        lv_obj_set_size(screenRoot, LV_PCT(100), LV_PCT(100));
        lv_obj_set_style_border_width(screenRoot, 0, 0);
        lv_obj_set_style_radius(screenRoot, 0, 0);
        lv_obj_set_style_pad_all(screenRoot, 0, 0);
        lv_obj_set_style_bg_opa(screenRoot, LV_OPA_TRANSP, 0);
        lv_obj_remove_flag(screenRoot, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
        screenTable[i]->create(screenRoot);
        logDebug("Created screen: " + std::to_string(i), "UI");
        screenTable[i]->hide();
        logDebug("Hid screen: " + std::to_string(i), "UI");
    }
    screenTable[toIndex(current)]->show();
    logDebug("Showed screen: " + std::to_string(toIndex(current)), "UI");
    lv_screen_load(root);
    logInfo("ScreenManager initialized", "UI");
}

void ScreenManager::set(ScreenId id) {
    if (id == current) return;
    if (toIndex(id) >= screenCount) return;

    screenTable[toIndex(current)]->hide();
    current = id;
    screenTable[toIndex(current)]->show();
}

void ScreenManager::update(const AppState& appState) {
    screenTable[toIndex(current)]->update(appState);
}
