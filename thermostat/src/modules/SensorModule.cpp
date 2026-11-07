#include "modules/SensorModule.h"
#include "utils/logging.h"

SensorModule::SensorModule(MqttModule& mqttModule, BME680& bme680) : _mqttModule(mqttModule), _bme680(bme680) {}

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
            _mqttModule.publish("sensor/temp", std::to_string(_temp));
            _mqttModule.publish("sensor/hum", std::to_string(_hum));
            _mqttModule.publish("sensor/pres", std::to_string(_pres));
            _mqttModule.publish("sensor/iaq", std::to_string(_iaq));
            _mqttModule.publish("sensor/iaq_acc", std::to_string(_iaqAccuracy));
        } else if (success == BME680ReadResult::SKIPPED) {
            logWarning("Sensor read skipped (called too fast).", "SensorModule");
        } else {
            logError("Failed to read sensor data.", "SensorModule");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 1hz
    }
}
