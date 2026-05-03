#pragma once

#include "modules/ui/screens/Screen.h"

class HomeScreen : public Screen {
public:
    void create(lv_obj_t* root) override;
    void show() override;
    void hide() override;
    void update(const AppState& appState) override;
private:
    lv_obj_t* tempLabel = nullptr;
    lv_obj_t* modeLabel = nullptr;
    lv_obj_t* heatSetpointLabel = nullptr;
    lv_obj_t* coolSetpointLabel = nullptr;
};