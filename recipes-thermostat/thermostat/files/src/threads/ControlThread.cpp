#include "threads/ControlThread.h"
#include <chrono>
#include <thread>

ControlThread::ControlThread(ConfigModule &config, HvacModule &hvac): _config{config}, _hvac{hvac} {}

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
