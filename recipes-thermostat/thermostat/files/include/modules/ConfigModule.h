#pragma once

#include <string>
#include <variant>
#include <mutex>
#include "utils/AppState.h"

enum class UpdateType {
    SensorTemp,
    SensorHum,
    SensorPres,
    SensorIAQ,
    SensorIAQAcc,
};

struct Update {
    UpdateType type;
    std::variant<int, float, std::string> data;
};

class ConfigModule {
public:
    ConfigModule();
    ~ConfigModule();

    void update(const Update msg);
    const AppState &snapshot();
    void syncState();
private:
    std::mutex _stateLock;
    AppState _appStateA; // Reading snapshot
    AppState _appStateB; // State to be written to
};