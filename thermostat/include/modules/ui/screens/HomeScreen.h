#pragma once

#include "modules/ui/screens/Screen.h"

class HomeScreen final : public Screen {
public:
    void create(lv_obj_t* root) override;
    void show() override;
    void hide() override;
    void update(const AppState& appState) override;

    int getHeatSetpoint() const { return heatSetpoint; }
    int getCoolSetpoint() const { return coolSetpoint; }
private:
    lv_obj_t* border = nullptr;

    lv_obj_t* menuButton = nullptr;
    lv_obj_t* hvacModeButton = nullptr;
    lv_obj_t* modeLabel = nullptr;

    lv_obj_t* tempLabel = nullptr;
    lv_obj_t* humidityLabel = nullptr;
    lv_obj_t* pressureLabel = nullptr;

    lv_obj_t* heatSetpointButton = nullptr;
    lv_obj_t* coolSetpointButton = nullptr;
    lv_obj_t* heatLabel = nullptr;
    lv_obj_t* coolLabel = nullptr;
    int heatSetpoint = 225;
    int coolSetpoint = 240;

};