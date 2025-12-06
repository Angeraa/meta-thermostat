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
        BME680ReadResult success = _bme680.readSensorData(temp, hum, pres, iaq, iaqAccuracy);
        std::cout << "[SensorModule] Reading sensor data..." << std::endl;
        if (success == BME680ReadResult::SUCCESS) {
            _mqttModule.publish("sensor/temperature", std::to_string(temp));
            _mqttModule.publish("sensor/humidity", std::to_string(hum));
            _mqttModule.publish("sensor/pressure", std::to_string(pres));
            _mqttModule.publish("sensor/iaq", std::to_string(iaq));
            _mqttModule.publish("sensor/iaqAccuracy", std::to_string(iaqAccuracy));
        } else if (success == BME680ReadResult::SKIPPED) {
            std::cout << "[SensorModule] Sensor read skipped (called too fast)." << std::endl;
        } else {
            std::cerr << "[SensorModule] Failed to read sensor data." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 2hz as read function will skip if called too fast
    }
}
