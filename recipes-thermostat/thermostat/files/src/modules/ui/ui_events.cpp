#include "modules/ui/ui_events.h"
#include "threads/UIThread.h"
#include <mutex>

static UIThread* uiThreadObjHandle = nullptr;
static std::mutex uiThreadMutex;

void registerUIThread(UIThread* ui) {
    std::lock_guard<std::mutex> lock(uiThreadMutex);
    uiThreadObjHandle = ui;
}

void unregisterUIThread() {
    std::lock_guard<std::mutex> lock(uiThreadMutex);
    uiThreadObjHandle = nullptr;
}

void postUIEvent(const UIEvent& e) {
    std::lock_guard<std::mutex> lock(uiThreadMutex);
    if (uiThreadObjHandle) {
        uiThreadObjHandle->postEvent(e);
    }
}