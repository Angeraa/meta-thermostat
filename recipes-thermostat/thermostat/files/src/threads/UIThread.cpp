#include "threads/UIThread.h"
#include "utils/AppState.h"

UIThread::UIThread(ConfigModule& config, ScreenManager& screenManager) : config(config), screenManager(screenManager) {
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
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
