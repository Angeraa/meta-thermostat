#include "threads/ControlThread.h"
#include <chrono>
#include <thread>

ControlThread::ControlThread(HvacModule &hvac, ConfigModule &config): _hvac{hvac}, _config{config} {}

ControlThread::~ControlThread() {
    stop();
}

void ControlThread::run() {
    while (_running) {
        AppState state = _config.snapshot();

        _hvac.loop(state);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 1Hz
    }
}

void ControlThread::stop() {
    _running.store(false);
}
