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
        lv_obj_t* screenRoot = lv_obj_create(root);
        lv_obj_set_size(screenRoot, LV_PCT(100), LV_PCT(100));
        lv_obj_set_style_border_width(screenRoot, 0, 0);
        lv_obj_set_style_radius(screenRoot, 0, 0);
        lv_obj_set_style_pad_all(screenRoot, 0, 0);
        lv_obj_set_style_bg_opa(screenRoot, LV_OPA_TRANSP, 0);
        lv_obj_remove_flag(screenRoot, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
        screenTable[i]->create(screenRoot);
        screenTable[i]->hide();
    }
    screenTable[toIndex(screenStack[stackPointer])]->show();
    lv_screen_load(root);
    logInfo("ScreenManager initialized", "UI");
}

void ScreenManager::set(ScreenId id) {
    if (id == screenStack[stackPointer]) return;
    if (toIndex(id) >= screenCount) return;

    for (int i = stackPointer; i >= 0; i--) {
        if (screenStack[i] == id) {
            screenTable[toIndex(screenStack[stackPointer])]->hide();
            stackPointer = i;
            screenTable[toIndex(screenStack[stackPointer])]->show();
            return;
        }
    }

    screenTable[toIndex(screenStack[stackPointer])]->hide();
    stackPointer++;
    screenStack[stackPointer] = id;
    screenTable[toIndex(screenStack[stackPointer])]->show();
}

void ScreenManager::back() {
    if (stackPointer == 0) return;
    screenTable[toIndex(screenStack[stackPointer])]->hide();
    stackPointer--;
    screenTable[toIndex(screenStack[stackPointer])]->show();
}

void ScreenManager::update(const AppState& appState) {
    screenTable[toIndex(screenStack[stackPointer])]->update(appState);
}

void ScreenManager::openSetpointDialog(const UIEvent& e) {
    if (e.id != UIEventId::OpenSetpointDialog) return;
    if (!e.callback) {
        logError("No callback provided for OpenSetpointDialog event", "UI");
        return;
    }
    if (!e.context) {
        logError("No context provided for OpenSetpointDialog event", "UI");
        return;
    }
    eventCallback = e.callback;
    eventContext = e.context;
    static_cast<SetpointScreen*>(screenTable[toIndex(ScreenId::Setpoint)])->configure(e.value, 150, 300);
    set(ScreenId::Setpoint);
}

void ScreenManager::setpointSelected(int temperature) {
    auto callback = eventCallback;
    auto context = eventContext;

    eventCallback = nullptr;
    eventContext = nullptr;
    if (callback && context) {
        callback(context, temperature);
    }
    back();
}
