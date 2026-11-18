#pragma once

enum class UIEventId {
    SwitchScreen,
    Back,
    OpenSetpointDialog,
    SetpointSelected,

    // State Change Events
    HeatSetpointChanged,
    CoolSetpointChanged,
    HvacModeChanged,
};

typedef void (*UIEventCallback)(void*, int);

struct UIEvent {
    UIEventId id = UIEventId::SwitchScreen;
    int value = 0;
    UIEventCallback callback = nullptr;
    void* context = nullptr;
};