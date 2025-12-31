#pragma once

#include "modules/HvacModule.h"
#include "modules/ConfigModule.h"
#include <atomic>

class ControlThread {
public:
    ControlThread(HvacModule &hvac, ConfigModule &config);
    ~ControlThread();
    
    void run();
    void stop();
private:
    ConfigModule &_config;
    HvacModule &_hvac;

    std::atomic<bool> _running;
};