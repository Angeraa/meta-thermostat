#include "threads/UIThread.h"
#include "utils/AppState.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <fcntl.h>
#include <iostream>
#include <linux/input.h>
#include <stdexcept>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

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
    return {};
}

static bool hasInputBit(const unsigned long* bits, unsigned bit) {
    constexpr unsigned bitsPerWord = sizeof(unsigned long) * 8;
    return (bits[bit / bitsPerWord] & (1UL << (bit % bitsPerWord))) != 0;
}

static std::string findTouchscreenDevice() {
    namespace fs = std::filesystem;
    const fs::path inputDirectory = "/dev/input";
    std::error_code ec;

    if (!fs::exists(inputDirectory, ec)) {
        return {};
    }

    for (const auto& entry : fs::directory_iterator(inputDirectory, ec)) {
        const std::string name = entry.path().filename().string();
        if (name.rfind("event", 0) != 0) {
            continue;
        }

        const int fd = ::open(entry.path().c_str(), O_RDONLY | O_NONBLOCK);
        if (fd < 0) {
            continue;
        }

        unsigned long eventBits[(EV_MAX / (sizeof(unsigned long) * 8)) + 1]{};
        unsigned long absBits[(ABS_MAX / (sizeof(unsigned long) * 8)) + 1]{};
        bool isTouchscreen = false;

        if (::ioctl(fd, EVIOCGBIT(0, sizeof(eventBits)), eventBits) >= 0 &&
            hasInputBit(eventBits, EV_ABS) &&
            hasInputBit(eventBits, EV_KEY) &&
            ::ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absBits)), absBits) >= 0) {
            // LVGL's evdev driver calibrates ABS_X/ABS_Y. Do not select a
            // device exposing only ABS_MT_POSITION_X/Y because those axes
            // would be read without valid calibration by that driver.
            const bool hasX = hasInputBit(absBits, ABS_X);
            const bool hasY = hasInputBit(absBits, ABS_Y);
            unsigned long keyBits[(KEY_MAX / (sizeof(unsigned long) * 8)) + 1]{};
            if (::ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keyBits)), keyBits) >= 0) {
                const bool hasTouchButton = hasInputBit(keyBits, BTN_TOUCH) ||
                                            hasInputBit(keyBits, BTN_TOOL_FINGER);
                isTouchscreen = hasX && hasY && hasTouchButton;
            }
        }

        ::close(fd);
        if (isTouchscreen) {
            return entry.path().string();
        }
    }

    return {};
}

UIThread::UIThread(ConfigModule& config, ScreenManager& screenManager) : config(config), screenManager(screenManager) {
    lv_init();
    lvglInitialized = true;
    lv_tick_set_cb(lvgl_tick_cb);

    const std::string cardPath = findDrmCard();
    if (cardPath.empty() || !std::filesystem::exists(cardPath)) {
        cleanupLvgl();
        throw std::runtime_error("[UIThread] No usable connected DRM display found");
    }

    display = lv_linux_drm_create();
    if (!display) {
        cleanupLvgl();
        throw std::runtime_error("[UIThread] Failed to create DRM display");
    }
    std::cout << "[UIThread] Using DRM device: " << cardPath << std::endl;
    lv_linux_drm_set_file(display, cardPath.c_str(), -1);

    const std::string inputPath = findTouchscreenDevice();
    if (inputPath.empty()) {
        cleanupLvgl();
        throw std::runtime_error("[UIThread] No touchscreen input device found");
    }
    std::cout << "[UIThread] Using input device: " << inputPath << std::endl;
    inputDevice = lv_evdev_create(LV_INDEV_TYPE_POINTER, inputPath.c_str());
    if (!inputDevice) {
        cleanupLvgl();
        throw std::runtime_error("[UIThread] Failed to initialize touchscreen input device");
    }
    lv_indev_set_display(inputDevice, display);

    screenManager.init();
    _running.store(true);
}

UIThread::~UIThread() {
    stop();   
    cleanupLvgl();
}

void UIThread::cleanupLvgl() {
    if (inputDevice) {
        lv_evdev_delete(inputDevice);
        inputDevice = nullptr;
    }
    if (display) {
        lv_display_delete(display);
        display = nullptr;
    }
    if (lvglInitialized) {
        lv_deinit();
        lvglInitialized = false;
    }
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
        const uint32_t nextDelayMs = lv_timer_handler();
        // Keep the UI responsive even when LVGL has no pending timer and
        // reports LV_NO_TIMER_READY (0xFFFFFFFF).
        const auto sleepMs = std::clamp(nextDelayMs, 1U, 100U);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
    }
}
