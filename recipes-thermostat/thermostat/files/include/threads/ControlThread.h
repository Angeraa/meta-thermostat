#pragma once

#include "modules/HvacModule.h"
#include "modules/ConfigModule.h"
#include <atomic>

class ControlThread {
public:
    ControlThread(ConfigModule &config, HvacModule &hvac);
    ~ControlThread();
    
    void run();
    void stop();
private:
    ConfigModule &_config;
    HvacModule &_hvac;

    std::atomic<bool> _running;
};