#pragma once

#include <cstddef>

enum class HvacState : size_t { 
    OFF = 0, 
    HEATING, 
    COOLING,
};

enum class HvacMode : size_t {
    OFF = 0,
    HEAT,
    COOL,
    AUTO,
};

struct AppState {
    // HVAC States with some defaults
    HvacMode mode = HvacMode::AUTO;
    HvacState state = HvacState::OFF;
    int currTemp = 0;
    int currHum = 0;
    float currPressure = 0.0f;
    float currIAQ = 0.0f;
    float currIAQAcc = 0.0f;
    int coolSetpoint = 240;
    int heatSetpoint = 225;

    int minCompOnTimeSeconds = 300;
    int minHeatOnTimeSeconds = 300;
    int minCompOffTimeSeconds = 60;
    int minHeatOffTimeSeconds = 60;
    int tempHysteresisDelta = 5;

    AppState(const AppState&) = default;
    AppState &operator=(const AppState&) = default;

    AppState(AppState&&) = delete;
    AppState& operator=(AppState&&) = delete;
};