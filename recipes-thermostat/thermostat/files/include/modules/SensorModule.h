#pragma once

#include "MQTTModule.h"
#include "hardware/bme680.h"
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
    float temp, hum, pres, iaq, iaqAccuracy;

    void controlLoop();
};
