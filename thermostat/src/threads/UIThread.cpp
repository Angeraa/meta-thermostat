#include "threads/UIThread.h"
#include "utils/logging.h"
#include "utils/AppState.h"
#include "lvgl/lvgl.h"
#include <algorithm>
#include <exception>
#include <filesystem>
#include <fstream>
#include <fcntl.h>
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

struct TouchscreenDevice {
    std::string path;
    input_absinfo x{};
    input_absinfo y{};
};

static TouchscreenDevice findTouchscreenDevice() {
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
        TouchscreenDevice device;

        if (::ioctl(fd, EVIOCGBIT(0, sizeof(eventBits)), eventBits) >= 0 &&
            hasInputBit(eventBits, EV_ABS) &&
            hasInputBit(eventBits, EV_KEY) &&
            ::ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(absBits)), absBits) >= 0) {
            const bool hasSingleTouch = hasInputBit(absBits, ABS_X) && hasInputBit(absBits, ABS_Y);
            const bool hasMultiTouch = hasInputBit(absBits, ABS_MT_POSITION_X) &&
                                       hasInputBit(absBits, ABS_MT_POSITION_Y);
            unsigned long keyBits[(KEY_MAX / (sizeof(unsigned long) * 8)) + 1]{};
            if ((hasSingleTouch || hasMultiTouch) &&
                ::ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keyBits)), keyBits) >= 0 &&
                hasInputBit(keyBits, BTN_TOUCH)) {
                const unsigned xAxis = hasMultiTouch ? ABS_MT_POSITION_X : ABS_X;
                const unsigned yAxis = hasMultiTouch ? ABS_MT_POSITION_Y : ABS_Y;
                if (::ioctl(fd, EVIOCGABS(xAxis), &device.x) >= 0 &&
                    ::ioctl(fd, EVIOCGABS(yAxis), &device.y) >= 0 &&
                    device.x.maximum > device.x.minimum &&
                    device.y.maximum > device.y.minimum) {
                    input_absinfo singleX{}, singleY{};
                    if (!hasSingleTouch || !hasMultiTouch ||
                        (::ioctl(fd, EVIOCGABS(ABS_X), &singleX) >= 0 &&
                         ::ioctl(fd, EVIOCGABS(ABS_Y), &singleY) >= 0 &&
                         singleX.minimum == device.x.minimum && singleX.maximum == device.x.maximum &&
                         singleY.minimum == device.y.minimum && singleY.maximum == device.y.maximum)) {
                        device.path = entry.path().string();
                    }
                }
            }
        }

        ::close(fd);
        if (!device.path.empty()) {
            return device;
        }
    }

    return {};
}

UIThread::UIThread(ConfigModule& config, ScreenManager& screenManager) : config(config), screenManager(screenManager) {}

void UIThread::initLvgl() {
    lv_init();
    lvglInitialized = true;
    lv_tick_set_cb(lvgl_tick_cb);

    const std::string cardPath = findDrmCard();
    if (cardPath.empty() || !std::filesystem::exists(cardPath)) {
        throw std::runtime_error("[UIThread] No usable connected DRM display found");
    }

    display = lv_linux_drm_create();
    if (!display) {
        throw std::runtime_error("[UIThread] Failed to create DRM display");
    }
    
    // Explicitly set display res before initializing draw buffers else seg fault
    lv_display_set_resolution(display, 720, 1280);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_XRGB8888);

    lv_theme_t * th = lv_theme_default_init(
        display,     // Display
        lv_palette_main(LV_PALETTE_BLUE), // Primary color
        lv_palette_main(LV_PALETTE_RED),  // Secondary color
        true,                      // Dark mode: true = dark theme
        LV_FONT_DEFAULT            // Default font
    );
    lv_disp_set_theme(display, th);

    logInfo("Using DRM device: " + cardPath, "UIThread");
    lv_linux_drm_set_file(display, cardPath.c_str(), -1);

    const lv_draw_buf_t* drawBuffer = lv_display_get_buf_active(display);
    if (!drawBuffer) {
        throw std::runtime_error("[UIThread] Failed to configure DRM display");
    }
    const int32_t displayWidth = lv_display_get_horizontal_resolution(display);
    const int32_t displayHeight = lv_display_get_vertical_resolution(display);
    logInfo("Display: " + std::to_string(displayWidth) + "x" + std::to_string(displayHeight) +
                ", buffer: " + std::to_string(drawBuffer->header.w) + "x" +
                std::to_string(drawBuffer->header.h) + ", stride: " +
                std::to_string(drawBuffer->header.stride) + ", bytes: " +
                std::to_string(drawBuffer->data_size) + ", color format: " +
                std::to_string(static_cast<unsigned>(drawBuffer->header.cf)),
            "UIThread");
    if (drawBuffer->header.w != static_cast<uint32_t>(displayWidth) ||
        drawBuffer->header.h != static_cast<uint32_t>(displayHeight) ||
        static_cast<uint64_t>(drawBuffer->header.stride) * displayHeight > drawBuffer->data_size) {
        throw std::runtime_error("[UIThread] DRM display and draw buffer layout do not match");
    }

    const TouchscreenDevice touchscreen = findTouchscreenDevice();
    if (touchscreen.path.empty()) {
        throw std::runtime_error("[UIThread] No touchscreen input device found");
    }
    logInfo("Using input device: " + touchscreen.path, "UIThread");
    inputDevice = lv_evdev_create(LV_INDEV_TYPE_POINTER, touchscreen.path.c_str());
    if (!inputDevice) {
        throw std::runtime_error("[UIThread] Failed to initialize touchscreen input device");
    }
    lv_indev_set_display(inputDevice, display);
    lv_evdev_set_calibration(inputDevice, touchscreen.x.minimum, touchscreen.y.minimum,
                             touchscreen.x.maximum, touchscreen.y.maximum);

    screenManager.init();
}

UIThread::~UIThread() {
    stop();
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
        logWarning("Event queue full, event dropped", "UIThread");
    }
}

void UIThread::run() {
    try {
        initLvgl();
    } catch (...) {
        cleanupLvgl();
        startup.set_exception(std::current_exception());
        return;
    }
    startup.set_value();

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
    cleanupLvgl();
}
