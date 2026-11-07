#include "modules/ConfigModule.h"
#include "utils/logging.h"

ConfigModule::ConfigModule() {
    // When non-volatile storage is added, load persisted state here, file handle etc.
}

ConfigModule::~ConfigModule() {
    // Cleanup any stuff that was added in constructor like file handles or maybe save state before closing some debug
    // logging of final state
}

void ConfigModule::update(const Message msg) {
    std::lock_guard<std::mutex> lock(_stateMutex);
    size_t inactive = 1ul - _activeBuffer;
    switch (msg.type) {
    case MessageType::SensorTemp: {
        auto value = std::get_if<int>(&msg.data);
        if (!value) {
            logError("Temp value expects 'int'", "ConfigModule");
            return;
        }
        _appStateBuffers[inactive].currTemp = *value;
        break;
    }
    case MessageType::SensorHum: {
        auto value = std::get_if<int>(&msg.data);
        if (!value) {
            logError("Humidity value expects 'int'", "ConfigModule");
            return;
        }
        _appStateBuffers[inactive].currHum = *value;
        break;
    }
    case MessageType::SensorPres: {
        auto value = std::get_if<float>(&msg.data);
        if (!value) {
            logError("Pressure value expects 'float'", "ConfigModule");
            return;
        }
        _appStateBuffers[inactive].currPressure = *value;
        break;
    }
    case MessageType::SensorIAQ: {
        auto value = std::get_if<float>(&msg.data);
        if (!value) {
            logError("IAQ value expects 'float'", "ConfigModule");
            return;
        }
        _appStateBuffers[inactive].currIAQ = *value;
        break;
    }
    case MessageType::SensorIAQAcc: {
        auto value = std::get_if<float>(&msg.data);
        if (!value) {
            logError("IAQ Accuracy value expects 'float'", "ConfigModule");
            return;
        }
        _appStateBuffers[inactive].currIAQAcc = *value;
        break;
    }
    case MessageType::HVACState: {
        auto value = std::get_if<size_t>(&msg.data);
        if (!value) {
            logError("HVAC State value expects 'size_t'", "ConfigModule");
            return;
        }
        _appStateBuffers[inactive].mode = *value;
        break;
    }
    default:
        logError("Unknown update type", "ConfigModule");
        break;
    }
}

AppState ConfigModule::snapshot() {
    std::lock_guard<std::mutex> lock(_stateMutex);
    return _appStateBuffers[_activeBuffer];
}

void ConfigModule::syncState() {
    std::lock_guard<std::mutex> lock(_stateMutex);
    _activeBuffer = 1ul - _activeBuffer;
    _appStateBuffers[1ul - _activeBuffer] = _appStateBuffers[_activeBuffer];
}