#pragma once

#include "modules/HvacModule.h"
#include "modules/ConfigModule.h"
#include "utils/IntentQueue.h"
#include <atomic>

class ControlThread {
public:
    ControlThread(HvacModule& hvac, ConfigModule& config, IntentQueue* intentQueue);
    ~ControlThread();
    
    void run();
    void stop();
private:
    ConfigModule &_config;
    HvacModule &_hvac;
    IntentQueue* _intentQueue;
    HvacState _lastState = HvacState::OFF;

    std::atomic<bool> _running;
};