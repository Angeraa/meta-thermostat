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
    lv_obj_t* humidityLabel = nullptr;
    lv_obj_t* pressureLabel = nullptr;
    lv_obj_t* modeLabel = nullptr;
    lv_obj_t* stateLabel = nullptr;
    lv_obj_t* heatLabel = nullptr;
    lv_obj_t* coolLabel = nullptr;
    lv_obj_t* menuButton = nullptr;
    lv_obj_t* border = nullptr;
};