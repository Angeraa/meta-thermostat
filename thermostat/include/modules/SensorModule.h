#pragma once

#include "utils/IntentQueue.h"
#include "hardware/BME680.h"
#include <string>
#include <thread>
#include <atomic>

class SensorModule {
public:
    SensorModule(IntentQueue* intentQueue, BME680& bme680);
    ~SensorModule() noexcept;
    void start();
    void stop();

private:
    IntentQueue* _intentQueue;
    std::thread _sensorThread;
    std::atomic<bool> _running;
    BME680& _bme680;
    int _tempOffset = 50; // Sensor reads higher than actual by about 5C
    int _temp, _hum;
    float _pres, _iaq, _iaqAccuracy;

    void controlLoop();
};
