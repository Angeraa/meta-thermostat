#pragma once

#include "MQTTModule.h"
#include "hardware/BME680.h"
#include <string>
#include <thread>
#include <atomic>

class SensorModule {
public:
    SensorModule(MqttModule& mqttModule, BME680& bme680);
    ~SensorModule() noexcept;
    void start();
    void stop();

private:
    MqttModule& _mqttModule;
    std::thread _sensorThread;
    std::atomic<bool> _running;
    BME680 _bme680;
    int _tempOffset = 50; // Sensor reads higher than actual by about 5C
    int _temp, _hum;
    float _pres, _iaq, _iaqAccuracy;

    void controlLoop();
};
