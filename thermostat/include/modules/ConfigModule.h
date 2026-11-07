#pragma once

#include <mutex>
#include "utils/AppState.h"
#include "utils/messages.h"

class ConfigModule {
public:
    ConfigModule();
    ~ConfigModule();

    void update(const Message msg);
    AppState snapshot();
    void syncState();
private:
    std::mutex _stateMutex;
    AppState _appStateBuffers[2]{{}, {}};
    size_t _activeBuffer{0};
};