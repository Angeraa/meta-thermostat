#pragma once

#include <cstddef>

struct AppState {
    // HVAC States with some defaults
    size_t mode = 0;
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