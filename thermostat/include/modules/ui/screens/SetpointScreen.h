#pragma once

#include "Screen.h"

class SetpointScreen final : public Screen {
public:
    void create(lv_obj_t *root) override;
    void show() override;
    void hide() override;
    void update(const AppState& appState) override;
    void configure(int setpoint, int min, int max);

    int currentSetpoint = 0;
private:
    lv_obj_t* setpointLabel = nullptr;
    lv_obj_t* setpointArcSlider = nullptr;

    int minSetpoint = 0;
    int maxSetpoint = 0;
};