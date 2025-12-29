#include "modules/SensorModule.h"

SensorModule::SensorModule(MqttModule& mqttModule, BME680& bme680) : _mqttModule(mqttModule), _bme680(bme680) {}

void SensorModule::start() {
    std::cout << "[SensorModule] Starting Sensor Module..." << std::endl;
    _running = true;
    _sensorThread = std::thread(&SensorModule::controlLoop, this);
}

void SensorModule::stop() {
    std::cout << "[SensorModule] Stopping Sensor Module..." << std::endl;
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
        float fTemp, fHum;
        BME680ReadResult success = _bme680.readSensorData(fTemp, fHum, _pres, _iaq, _iaqAccuracy);
        std::cout << "[SensorModule] Reading sensor data..." << std::endl;
        if (success == BME680ReadResult::SUCCESS) {
            _temp = static_cast<int>(fTemp * 10) - _tempOffset; // Saving these values in class memory for some later functions
            _hum = static_cast<int>(fHum); // ^^^
            _mqttModule.publish("sensor/temperature", std::to_string(_temp));
            _mqttModule.publish("sensor/humidity", std::to_string(_hum));
            _mqttModule.publish("sensor/pressure", std::to_string(_pres));
            _mqttModule.publish("sensor/iaq", std::to_string(_iaq));
            _mqttModule.publish("sensor/iaqAccuracy", std::to_string(_iaqAccuracy));
        } else if (success == BME680ReadResult::SKIPPED) {
            std::cout << "[SensorModule] Sensor read skipped (called too fast)." << std::endl;
        } else {
            std::cerr << "[SensorModule] Failed to read sensor data." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 1hz
    }
}
