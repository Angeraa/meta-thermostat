#pragma once

#include "modules/ui/screens/Screen.h"

class ScreenManager {
public:
    void init();
    void set(ScreenId id);
    void update(const AppState& appState);

private:
    lv_obj_t* root = nullptr;
    ScreenId current = ScreenId::Home;
};