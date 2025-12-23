#pragma once

#include "hardware/GPIOManager.h"
#include "utils/AppState.h"
#include <chrono>
#include <iostream>

#define HVAC_HEATING_PIN 23
#define HVAC_COOLING_PIN 24

enum class HvacState { OFF, HEATING, COOLING };

class HvacModule {
public:
    HvacModule();
    ~HvacModule();

    // Main loop function
    void loop(AppState &appState);
private:
    std::mutex _stateMutex;

    HvacState _currentState = HvacState::OFF;
    std::chrono::steady_clock::time_point _currentStateStart = std::chrono::steady_clock::now();
};
