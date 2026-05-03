#include "threads/UIThread.h"
#include "utils/AppState.h"
#include <iostream>

UIThread::UIThread(ConfigModule& config, ScreenManager& screenManager) : config(config), screenManager(screenManager) {
    lv_init();

    display = lv_linux_drm_create();
    lv_linux_drm_set_file(display, "/dev/dri/card0", -1);

    inputDevice = lv_libinput_create(LV_INDEV_TYPE_POINTER, "/dev/input/event0");
    if (!inputDevice) {
        std::cout << "[UIThread] Warning: Failed to initialize input device, touchscreen input will not work" << std::endl;
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
    queue.push(e);
}

void UIThread::run() {
    while (_running) {
        UIEvent event;
        while (queue.pop(event)) {
            // Thinking of switching on event and having a handler function for each event type like state change, switch screen, etc
        }

        screenManager.update(config.snapshot());
        lv_timer_handler();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
