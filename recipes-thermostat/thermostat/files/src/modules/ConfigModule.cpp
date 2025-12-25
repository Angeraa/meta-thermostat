#include "modules/ConfigModule.h"
#include <iostream>

void ConfigModule::update(const Message msg) {
    std::lock_guard<std::mutex> lock(_stateLock);

    switch (msg.type) {
        case MessageType::SensorTemp: {
            auto value = std::get_if<int>(&msg.data);
            if (!value) {
                std::cout << "[CONFIG] Critical: Temp value expects 'int'" << std::endl;
                return;
            }
            _appStateB.currTemp = *value;
            break;
        }
        case MessageType::SensorHum: {
            auto value = std::get_if<int>(&msg.data);
            if (!value) {
                std::cout << "[CONFIG] Critical: Humidity value expects 'int'" << std::endl;
                return;
            }
            _appStateB.currHum = *value;
            break;
        }
        case MessageType::SensorPres: {
            auto value = std::get_if<float>(&msg.data);
            if (!value) {
                std::cout << "[CONFIG] Critical: Pressure value expects 'float'" << std::endl;
                return;
            }
            _appStateB.currPressure = *value;
            break;
        }
        case MessageType::SensorIAQ: {
            auto value = std::get_if<float>(&msg.data);
            if (!value) {
                std::cout << "[CONFIG] Critical: IAQ value expects 'float'" << std::endl;
                return;
            }
            _appStateB.currIAQ = *value;
            break;
        }
        case MessageType::SensorIAQAcc: {
            auto value = std::get_if<float>(&msg.data);
            if (!value) {
                std::cout << "[CONFIG] Critical: IAQ Accuracy value expects 'float'" << std::endl;
                return;
            }
            _appStateB.currIAQAcc = *value;
            break;
        }
        default:
            std::cout << "[CONFIG] Critical: Unknown update type" << std::endl;
            break;
    }
}

const AppState& ConfigModule::snapshot() {
    std::lock_guard<std::mutex> lock(_stateLock);
    return _appStateA;
}

void ConfigModule::syncState() {
    std::lock_guard<std::mutex> lock(_stateLock);
    _appStateA = _appStateB;
}