#pragma once

#include "hardware/GPIOManager.h"
#include <chrono>
#include <iostream>

#define HVAC_HEATING_PIN 23
#define HVAC_COOLING_PIN 24

enum class HvacState { OFF, HEATING, COOLING };

class HvacController {
public:
    HvacController();
    ~HvacController();

    
    void setCurrentTemp(int newTemp);
    void setHeatSetpoint(int newSetpoint);
    void setCoolSetpoint(int newSetpoint);

    int getHeatSetpoint();
    int getCoolSetpoint();
    HvacState getCurrentHvacState();

    // Main loop function
    void loop();

private:
    std::mutex _stateMutex;

    int _heatSetpoint = 225; // THESE VALUES ARE IN MULTIPLES OF 10 (E.G., 235 = 23.5C)
    int _coolSetpoint = 245; // ^^^
    int _currentTemp; // ^^^
    
    // true until change in current temp is below a defined threshold so hvac doesn't start immediately
    // unfortunately I don't know what the best threshold is going to be yet
    bool _isCalibrating = true;

    // Various hvac control parameters that will be configurable later once a UI is in place and will likely be in its
    // own object
    int _minCompOnTimeSeconds = 300;
    int _minHeatOnTimeSeconds = 300;
    int _minCompOffTimeSeconds = 60;
    int _minHeatOffTimeSeconds = 60;
    int _tempHysteresisDelta = 5; // Temperature delta to prevent rapid cycling, also in multiples of 10

    HvacState _currentState = HvacState::OFF;
    std::chrono::steady_clock::time_point _currentStateStart = std::chrono::steady_clock::now();
};
