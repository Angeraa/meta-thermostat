#pragma once

#include "modules/HvacModule.h"
#include "modules/ConfigModule.h"
#include "modules/MQTTModule.h"
#include <atomic>

class ControlThread {
public:
    ControlThread(HvacModule &hvac, ConfigModule &config, MqttModule &mqtt);
    ~ControlThread();
    
    void run();
    void stop();
private:
    ConfigModule &_config;
    HvacModule &_hvac;
    MqttModule &_mqtt;
    HvacState _lastState = HvacState::OFF;

    std::atomic<bool> _running;
};