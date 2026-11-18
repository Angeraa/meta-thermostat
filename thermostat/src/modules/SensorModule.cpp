#include "modules/SensorModule.h"
#include "utils/logging.h"

SensorModule::SensorModule(IntentQueue* intentQueue, BME680& bme680)
    : _intentQueue(intentQueue), _bme680(bme680) {}

void SensorModule::start() {
    logInfo("Starting Sensor Module...", "SensorModule");
    _running = true;
    _sensorThread = std::thread(&SensorModule::controlLoop, this);
}

void SensorModule::stop() {
    logInfo("Stopping Sensor Module...", "SensorModule");
    _running = false;
    if (_sensorThread.joinable()) {
        _sensorThread.join();
    }
}

SensorModule::~SensorModule() noexcept {
    stop();
}

void SensorModule::controlLoop() {
    while (_running) {
        float fTemp = 0.0f, fHum = 0.0f;
        BME680ReadResult success = BME680ReadResult::FAILURE;
        try {
            success = _bme680.readSensorData(fTemp, fHum, _pres, _iaq, _iaqAccuracy);
        } catch (const std::exception &e) {
            logError(e.what(), "SensorModule");
        }
        logDebug("Reading sensor data...", "SensorModule");
        if (success == BME680ReadResult::SUCCESS) {
            _temp = static_cast<int>(fTemp * 10) - _tempOffset; // Saving these values in class memory for some later functions
            _hum = static_cast<int>(fHum); // ^^^
            if (!_intentQueue) {
                logError("Intent queue unavailable", "SensorModule");
            } else {
                _intentQueue->publish({MessageType::SensorTemp, _temp});
                _intentQueue->publish({MessageType::SensorHum, _hum});
                _intentQueue->publish({MessageType::SensorPres, _pres});
                _intentQueue->publish({MessageType::SensorIAQ, _iaq});
                _intentQueue->publish({MessageType::SensorIAQAcc, _iaqAccuracy});
            }
        } else if (success == BME680ReadResult::SKIPPED) {
            logWarning("Sensor read skipped (called too fast).", "SensorModule");
        } else {
            logError("Failed to read sensor data.", "SensorModule");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 1hz
    }
}
