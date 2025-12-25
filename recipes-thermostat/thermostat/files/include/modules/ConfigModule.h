#pragma once

#include <mutex>
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
    std::mutex _stateLock;
    AppState _appStateA; // Reading snapshot
    AppState _appStateB; // State to be written to
};