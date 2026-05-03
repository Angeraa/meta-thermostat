#pragma once

#include "boost/lockfree/spsc_queue.hpp"
#include "modules/ui/UIEvent.h"
#include "modules/ui/ScreenManager.h"
#include "modules/ConfigModule.h"
#include <atomic>
#include <chrono>
#include <thread>
#include "lvgl/lvgl.h"

#define HOR_RES 720
#define VER_RES 1280

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

    lv_display_t* display;
    lv_indev_t* inputDevice;

    static uint8_t lvglBuffer[HOR_RES * VER_RES / 5]; // Partial

    std::atomic<bool> _running;
};