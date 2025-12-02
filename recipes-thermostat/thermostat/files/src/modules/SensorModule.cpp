#include "modules/SensorModule.h"

SensorModule::SensorModule(MqttModule& mqttModule) : _mqttModule(mqttModule) {}

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
        // bool success = _bme680.readSensorData(temp, hum, pres, gas);
        std::cout << "[SensorModule] Reading sensor data..." << std::endl;
        bool success = true; // Placeholder for actual sensor reading since we are testing MQTT functionality
        temp = 22.5; // Placeholder temperature
        hum = 45.0;  // Placeholder humidity
        pres = 1013.25; // Placeholder pressure
        gas = 120; // Placeholder gas resistance
        if (success) {
            _mqttModule.publish("sensor/temperature", std::to_string(temp));
            _mqttModule.publish("sensor/humidity", std::to_string(hum));
            _mqttModule.publish("sensor/pressure", std::to_string(pres));
            _mqttModule.publish("sensor/gas", std::to_string(gas));
        } else {
            std::cerr << "[SensorModule] Failed to read sensor data." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1)); // 1hz to match controller loop 
    }
}
