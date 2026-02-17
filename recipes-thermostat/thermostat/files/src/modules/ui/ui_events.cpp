#include "modules/ui/ui_events.h"

static UIThread* uiThreadHandle = nullptr;

void registerUIThread(UIThread* ui) {
    uiThreadHandle = ui;
}

void postUIEvent(const UIEvent& e) {
    if (uiThreadHandle) {
        uiThreadHandle->postEvent(e);
    }
}