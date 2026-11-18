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
        if (msg.payload != PayloadType::Int) {
            logError("Temp value expects 'int'", "ConfigModule");
            return;
        }
        _appStateBuffers[inactive].currTemp = msg.data.intValue;
        break;
    }
    case MessageType::SensorHum: {
        if (msg.payload != PayloadType::Int) {
            logError("Humidity value expects 'int'", "ConfigModule");
            return;
        }
        _appStateBuffers[inactive].currHum = msg.data.intValue;
        break;
    }
    case MessageType::SensorPres: {
        if (msg.payload != PayloadType::Float) {
            logError("Pressure value expects 'float'", "ConfigModule");
            return;
        }
        _appStateBuffers[inactive].currPressure = msg.data.floatValue;
        break;
    }
    case MessageType::SensorIAQ: {
        if (msg.payload != PayloadType::Float) {
            logError("IAQ value expects 'float'", "ConfigModule");
            return;
        }
        _appStateBuffers[inactive].currIAQ = msg.data.floatValue;
        break;
    }
    case MessageType::SensorIAQAcc: {
        if (msg.payload != PayloadType::Float) {
            logError("IAQ Accuracy value expects 'float'", "ConfigModule");
            return;
        }
        _appStateBuffers[inactive].currIAQAcc = msg.data.floatValue;
        break;
    }
    case MessageType::HVACState: {
        if (msg.payload != PayloadType::SizeT) {
            logError("HVAC State value expects 'size_t'", "ConfigModule");
            return;
        }
        _appStateBuffers[inactive].state = static_cast<HvacState>(msg.data.sizeValue);
        break;
    }
    case MessageType::HeatSetpoint: {
        if (msg.payload != PayloadType::Int) {
            logError("Heat setpoint value expects 'int'", "ConfigModule");
            return;
        }
        _appStateBuffers[inactive].heatSetpoint = msg.data.intValue;
        break;
    }
    case MessageType::CoolSetpoint: {
        if (msg.payload != PayloadType::Int) {
            logError("Cool setpoint value expects 'int'", "ConfigModule");
            return;
        }
        _appStateBuffers[inactive].coolSetpoint = msg.data.intValue;
        break;
    }
    case MessageType::HVACMode: {
        if (msg.payload != PayloadType::SizeT) {
            logError("HVAC Mode value expects 'size_t'", "ConfigModule");
            return;
        }
        _appStateBuffers[inactive].mode = static_cast<HvacMode>(msg.data.sizeValue);
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