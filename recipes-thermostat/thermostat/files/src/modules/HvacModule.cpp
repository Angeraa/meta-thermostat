#include "modules/HvacModule.h"

HvacModule::HvacModule() {
    // These pins are not going to be used anywhere else so requesting them from the start
    GPIOManager::getInstance().requestPin(HVAC_COOLING_PIN, "hvac", true);
    GPIOManager::getInstance().requestPin(HVAC_HEATING_PIN, "hvac", true);
    GPIOManager::getInstance().setPinValue(HVAC_COOLING_PIN, false);
    GPIOManager::getInstance().setPinValue(HVAC_HEATING_PIN, false);
}

HvacModule::~HvacModule() {
    GPIOManager::getInstance().releasePin(HVAC_COOLING_PIN, "hvac");
    GPIOManager::getInstance().releasePin(HVAC_HEATING_PIN, "hvac");
}

void HvacModule::loop(AppState &appState) {
    std::lock_guard<std::mutex> lock(_stateMutex);
    auto timeSinceStateChange = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - _currentStateStart).count();

    // Will have more than just heating, cooling, and off in the future like aux or two stage
    if (_currentState == HvacState::COOLING) {
        if (timeSinceStateChange >= appState.minCompOnTimeSeconds && appState.currTemp < appState.coolSetpoint - appState.tempHysteresisDelta) {
            // We have reached min on time so we have the ability to exit cooling mode
            // Transition to off first to not have both heating and cooling on at the same time
            _currentState = HvacState::OFF;
            _currentStateStart = std::chrono::steady_clock::now();
            GPIOManager::getInstance().setPinValue(HVAC_COOLING_PIN, false);
            std::cout << "[HVAC CONTROL] Switching to OFF" << std::endl;
        }
    } else if (_currentState == HvacState::HEATING) {
        if (timeSinceStateChange >= appState.minHeatOnTimeSeconds && appState.currTemp > appState.heatSetpoint + appState.tempHysteresisDelta) {
            // We have reached min on time so we have the ability to exit heating mode
            // Transition to off first to not have both heating and cooling on at the same time
            _currentState = HvacState::OFF;
            _currentStateStart = std::chrono::steady_clock::now();
            GPIOManager::getInstance().setPinValue(HVAC_HEATING_PIN, false);
            std::cout << "[HVAC CONTROL] Switching to OFF" << std::endl;
        }
    } else if (_currentState == HvacState::OFF) {
        if (appState.currTemp > appState.coolSetpoint && timeSinceStateChange >= appState.minCompOffTimeSeconds) {
            // We have reached min off time for cooling so we can enter cooling again
            _currentState = HvacState::COOLING;
            _currentStateStart = std::chrono::steady_clock::now();
            GPIOManager::getInstance().setPinValue(HVAC_COOLING_PIN, true);
            std::cout << "[HVAC CONTROL] Switching to COOLING" << std::endl;
        } else if (appState.currTemp < appState.heatSetpoint && timeSinceStateChange >= appState.minHeatOffTimeSeconds) {
            // We have reached min off time for heating so we can enter heating again
            _currentState = HvacState::HEATING;
            _currentStateStart = std::chrono::steady_clock::now();
            GPIOManager::getInstance().setPinValue(HVAC_HEATING_PIN, true);
            std::cout << "[HVAC CONTROL] Switching to HEATING" << std::endl;
        }
    }
}