#pragma once

struct AppState {
    // HVAC States with some defaults
    int currTemp;
    int currHum;
    float currPressure;
    float currIAQ;
    float currIAQAcc;
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