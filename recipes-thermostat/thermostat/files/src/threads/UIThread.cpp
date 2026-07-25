#include "threads/UIThread.h"
#include "utils/AppState.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

static uint32_t lvgl_tick_cb() {
    using namespace std::chrono;
    static const auto start = steady_clock::now();
    return static_cast<uint32_t>(
        duration_cast<milliseconds>(steady_clock::now() - start).count());
}

static std::string findDrmCard() {
    namespace fs = std::filesystem;
    std::error_code ec;
    const char* drmClassPath = "/sys/class/drm";
    if (fs::exists(drmClassPath, ec)) {
        for (const auto& entry : fs::directory_iterator(drmClassPath, ec)) {
            std::ifstream statusFile(entry.path() / "status");
            if (!statusFile.is_open()) continue;

            std::string status;
            std::getline(statusFile, status);
            if (status != "connected") continue;

            std::string name = entry.path().filename().string();
            auto dash = name.find('-');
            if (dash != std::string::npos) {
                return "/dev/dri/" + name.substr(0, dash);
            }
        }
    }
    return "/dev/dri/card0"; // fallback
}

UIThread::UIThread(ConfigModule& config, ScreenManager& screenManager) : config(config), screenManager(screenManager) {
    lv_init();
    lv_tick_set_cb(lvgl_tick_cb);

    display = lv_linux_drm_create();
    if (!display) {
        std::cout << "[UIThread] Error: Failed to create DRM display" << std::endl;
    } else {
        std::string cardPath = findDrmCard();
        std::cout << "[UIThread] Using DRM device: " << cardPath << std::endl;
        lv_linux_drm_set_file(display, cardPath.c_str(), -1);
    }

    const char* touchDev = lv_libinput_find_dev(LV_LIBINPUT_CAPABILITY_TOUCH, true);
    const char* inputPath = touchDev ? touchDev : "/dev/input/event0";
    std::cout << "[UIThread] Using input device: " << inputPath << std::endl;
    inputDevice = lv_libinput_create(LV_INDEV_TYPE_POINTER, inputPath);
    if (!inputDevice) {
        std::cout << "[UIThread] Warning: Failed to initialize input device, touchscreen input will not work" << std::endl;
    } else if (display) {
        lv_indev_set_display(inputDevice, display);
    }

    screenManager.init();
    _running.store(true);
}

UIThread::~UIThread() {
    stop();   
}

void UIThread::stop() {
    _running.store(false);
}

void UIThread::postEvent(const UIEvent& e) {
    if (!queue.push(e)) {
        std::cout << "[UIThread] Warning: event queue full, event dropped" << std::endl;
    }
}

void UIThread::run() {
    while (_running) {
        UIEvent event;
        while (queue.pop(event)) {
            switch (event.id) {
                case UIEventId::SwitchScreen: {
                    auto idx = static_cast<size_t>(event.value);
                    if (idx < screenCount) {
                        screenManager.set(static_cast<ScreenId>(idx));
                    }
                    break;
                }
            }
        }

        screenManager.update(config.snapshot());
        lv_timer_handler();
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
}
