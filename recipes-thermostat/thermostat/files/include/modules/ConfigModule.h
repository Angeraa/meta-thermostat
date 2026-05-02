#pragma once

#include <atomic>
#include "utils/AppState.h"
#include "utils/messages.h"

class ConfigModule {
public:
    ConfigModule();
    ~ConfigModule();

    void update(const Message msg);
    const AppState &snapshot();
    void syncState();
private:
    AppState _appStateBuffers[2]{{}, {}};
    std::atomic<size_t> _activeBuffer{0};
};