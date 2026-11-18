#pragma once

#include "modules/ui/screens/Screen.h"
#include "modules/ui/UIEvent.h"

constexpr uint8_t MAX_SCREEN_STACK_DEPTH = 8;

class ScreenManager {
public:
    void init();
    void set(ScreenId id);
    void back();
    void update(const AppState& appState);

protected:
    friend class UIThread;
    void openSetpointDialog(const UIEvent& e);
    void setpointSelected(int temperature);

    UIEventCallback eventCallback = nullptr;
    void* eventContext = nullptr;

private:
    lv_obj_t* root = nullptr;
    uint8_t stackPointer = 0;
    ScreenId screenStack[MAX_SCREEN_STACK_DEPTH] = {ScreenId::Home};
};