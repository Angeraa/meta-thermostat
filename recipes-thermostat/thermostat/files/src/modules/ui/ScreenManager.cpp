#include "ui/ScreenManager.h"
#include "ui/screenRegistry.h"

void ScreenManager::init() {
    root = lv_scr_act();
    for (auto i = 0; i < screenCount; i++) {
        screenTable[i]->create(root);
        screenTable[i]->hide();
    }
    screenTable[toIndex(current)]->show();
}

void ScreenManager::set(ScreenId id) {
    if (id == current) return;

    screenTable[toIndex(current)]->hide();
    current = id;
    screenTable[toIndex(current)]->show();
}

void ScreenManager::update(const AppState& appState) {
    screenTable[toIndex(current)]->update(appState);
}
