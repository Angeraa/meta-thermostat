#include "modules/ConfigModule.h"
#include <iostream>

ConfigModule::ConfigModule() {
    // When non-volatile storage is added, load persisted state here, file handle etc.
}

ConfigModule::~ConfigModule() {
    // Cleanup any stuff that was added in constructor like file handles or maybe save state before closing some debug
    // logging of final state
}

void ConfigModule::update(const Message msg) {
    size_t inactive = 1ul - _activeBuffer.load();
    switch (msg.type) {
    case MessageType::SensorTemp: {
        auto value = std::get_if<int>(&msg.data);
        if (!value) {
            std::cout << "[CONFIG] Critical: Temp value expects 'int'" << std::endl;
            return;
        }
        _appStateBuffers[inactive].currTemp = *value;
        break;
    }
    case MessageType::SensorHum: {
        auto value = std::get_if<int>(&msg.data);
        if (!value) {
            std::cout << "[CONFIG] Critical: Humidity value expects 'int'" << std::endl;
            return;
        }
        _appStateBuffers[inactive].currHum = *value;
        break;
    }
    case MessageType::SensorPres: {
        auto value = std::get_if<float>(&msg.data);
        if (!value) {
            std::cout << "[CONFIG] Critical: Pressure value expects 'float'" << std::endl;
            return;
        }
        _appStateBuffers[inactive].currPressure = *value;
        break;
    }
    case MessageType::SensorIAQ: {
        auto value = std::get_if<float>(&msg.data);
        if (!value) {
            std::cout << "[CONFIG] Critical: IAQ value expects 'float'" << std::endl;
            return;
        }
        _appStateBuffers[inactive].currIAQ = *value;
        break;
    }
    case MessageType::SensorIAQAcc: {
        auto value = std::get_if<float>(&msg.data);
        if (!value) {
            std::cout << "[CONFIG] Critical: IAQ Accuracy value expects 'float'" << std::endl;
            return;
        }
        _appStateBuffers[inactive].currIAQAcc = *value;
        break;
    }
    case MessageType::HVACState: {
        auto value = std::get_if<size_t>(&msg.data);
        if (!value) {
            std::cout << "[CONFIG] Critical: HVAC State value expects 'size_t'" << std::endl;
            return;
        }
        _appStateBuffers[inactive].mode = *value;
        break;
    }
    default:
        std::cout << "[CONFIG] Critical: Unknown update type" << std::endl;
        break;
    }
}

const AppState &ConfigModule::snapshot() {
    return _appStateBuffers[_activeBuffer.load()];
}

void ConfigModule::syncState() {
    // No issues with syncing as writes and syncs happen in the same thread always
    _activeBuffer.store(1ul - _activeBuffer.load());
    _appStateBuffers[1ul - _activeBuffer.load()] = _appStateBuffers[_activeBuffer.load()];
}