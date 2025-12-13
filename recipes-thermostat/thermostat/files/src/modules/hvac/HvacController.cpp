#include "modules/hvac/HvacController.h"

HvacController::HvacController() {
    // These pins are not going to be used anywhere else so requesting them from the start
    GPIOManager::getInstance().requestPin(HVAC_COOLING_PIN, "hvac", true);
    GPIOManager::getInstance().requestPin(HVAC_HEATING_PIN, "hvac", true);
}

HvacController::~HvacController() {
    GPIOManager::getInstance().releasePin(HVAC_COOLING_PIN, "hvac");
    GPIOManager::getInstance().releasePin(HVAC_HEATING_PIN, "hvac");
}

int HvacController::getCoolSetpoint() {
    std::lock_guard<std::mutex> lock(_stateMutex);
    return _coolSetpoint;
}

int HvacController::getHeatSetpoint() {
    std::lock_guard<std::mutex> lock(_stateMutex);
    return _heatSetpoint;
}

HvacState HvacController::getCurrentHvacState() {
    std::lock_guard<std::mutex> lock(_stateMutex);
    return _currentState;
}

void HvacController::setCoolSetpoint(int newSetpoint) {
    std::lock_guard<std::mutex> lock(_stateMutex);
    _coolSetpoint = newSetpoint;
}

void HvacController::setHeatSetpoint(int newSetpoint) {
    std::lock_guard<std::mutex> lock(_stateMutex);
    _heatSetpoint = newSetpoint;
}

void HvacController::setCurrentTemp(int newTemp) {
    std::lock_guard<std::mutex> lock(_stateMutex);
    if (abs(_currentTemp - newTemp) <= 1) _isCalibrating = false; // Stop 'calibration' if temp diff is 0.1C or less
    _currentTemp = newTemp;
}

void HvacController::loop() {
    std::lock_guard<std::mutex> lock(_stateMutex);

    if (_isCalibrating) return; // Skip if still calibrating since the default state is OFF

    auto timeSinceStateChange = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - _currentStateStart).count();

    // Will have more than just heating, cooling, and off in the future like aux or two stage
    if (_currentState == HvacState::COOLING) {
        if (timeSinceStateChange >= _minCompOnTimeSeconds && _currentTemp < _coolSetpoint - _tempHysteresisDelta) {
            // We have reached min on time so we have the ability to exit cooling mode
            // Transition to off first to not have both heating and cooling on at the same time
            _currentState = HvacState::OFF;
            _currentStateStart = std::chrono::steady_clock::now();
            GPIOManager::getInstance().setPinValue(HVAC_COOLING_PIN, false);
        }
    } else if (_currentState == HvacState::HEATING) {
        if (timeSinceStateChange >= _minHeatOnTimeSeconds && _currentTemp > _heatSetpoint + _tempHysteresisDelta) {
            // We have reached min on time so we have the ability to exit heating mode
            // Transition to off first to not have both heating and cooling on at the same time
            _currentState = HvacState::OFF;
            _currentStateStart = std::chrono::steady_clock::now();
            GPIOManager::getInstance().setPinValue(HVAC_HEATING_PIN, false);
        }
    } else if (_currentState == HvacState::OFF) {
        if (_currentTemp > _coolSetpoint && timeSinceStateChange >= _minCompOffTimeSeconds) {
            // We have reached min off time for cooling so we can enter cooling again
            _currentState = HvacState::COOLING;
            _currentStateStart = std::chrono::steady_clock::now();
            GPIOManager::getInstance().setPinValue(HVAC_COOLING_PIN, true);
        } else if (_currentTemp < _heatSetpoint && timeSinceStateChange >= _minHeatOffTimeSeconds) {
            // We have reached min off time for heating so we can enter heating again
            _currentState = HvacState::HEATING;
            _currentStateStart = std::chrono::steady_clock::now();
            GPIOManager::getInstance().setPinValue(HVAC_HEATING_PIN, true);
        }
    }
}