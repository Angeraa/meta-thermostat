#include "threads/ControlThread.h"
#include <chrono>
#include <thread>

ControlThread::ControlThread(HvacModule& hvac, ConfigModule& config, IntentQueue* intentQueue)
    : _hvac{hvac}, _config{config}, _intentQueue{intentQueue} {
    _running.store(true);
}

ControlThread::~ControlThread() {
    stop();
}

void ControlThread::run() {
    while (_running) {
        AppState state = _config.snapshot();

        _hvac.loop(state);
        if (_lastState != _hvac.getCurrentState() || state.state != _hvac.getCurrentState()) {
            _lastState = _hvac.getCurrentState();
            if (_intentQueue) {
                _intentQueue->publish({
                    MessageType::HVACState,
                    static_cast<size_t>(_lastState)
                });
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 1Hz
    }
}

void ControlThread::stop() {
    _running.store(false);
}
