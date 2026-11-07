#pragma once

#include "boost/lockfree/spsc_queue.hpp"
#include "modules/ui/UIEvent.h"
#include "modules/ui/ScreenManager.h"
#include "modules/ConfigModule.h"
#include <atomic>
#include <chrono>
#include <future>
#include <thread>
#include "lvgl/lvgl.h"

#define HOR_RES 800
#define VER_RES 480

class UIThread {
public:

    UIThread(ConfigModule& config, ScreenManager& screenManager);
    ~UIThread();

    void postEvent(const UIEvent& e);
    std::future<void> ready() { return startup.get_future(); }
    void run();
    void stop();

private:
    void initLvgl();
    void cleanupLvgl();

    boost::lockfree::spsc_queue<UIEvent, boost::lockfree::capacity<16>> queue;

    ConfigModule& config;
    ScreenManager& screenManager;

    lv_display_t* display = nullptr;
    lv_indev_t* inputDevice = nullptr;
    bool lvglInitialized = false;

    std::promise<void> startup;
    std::atomic<bool> _running{true};
};