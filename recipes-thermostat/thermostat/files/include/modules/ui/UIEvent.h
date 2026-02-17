#pragma once

enum class UIEventId {
    SwitchScreen,
};

struct UIEvent {
    UIEventId id;
    int value;
};