#pragma once

#include "boost/lockfree/spsc_queue.hpp"
#include "modules/ui/UIEvent.h"
#include "modules/ui/ScreenManager.h"
#include "modules/ConfigModule.h"
#include <atomic>
#include <chrono>
#include <thread>

class UIThread {
public:

    UIThread(ConfigModule& config, ScreenManager& screenManager);
    ~UIThread();

    void postEvent(const UIEvent& e);
    void run();
    void stop();

private:
    boost::lockfree::spsc_queue<UIEvent> queue{16};

    ConfigModule& config;
    ScreenManager& screenManager;

    std::atomic<bool> _running;
};