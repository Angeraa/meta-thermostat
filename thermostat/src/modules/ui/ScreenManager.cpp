#include "modules/ui/ScreenManager.h"
#include "modules/ui/screenRegistry.h"

void ScreenManager::init() {

    root = lv_obj_create(NULL);
    lv_obj_set_size(root, SCREEN_WIDTH, SCREEN_HEIGHT);
    for (auto i = 0; i < screenCount; i++) {
        screenTable[i]->create(root);
        screenTable[i]->hide();
    }
    screenTable[toIndex(current)]->show();
    lv_screen_load(root);
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
