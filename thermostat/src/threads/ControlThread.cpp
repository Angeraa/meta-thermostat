#include "threads/ControlThread.h"
#include <chrono>
#include <thread>

ControlThread::ControlThread(HvacModule &hvac, ConfigModule &config, MqttModule &mqtt): _hvac{hvac}, _config{config}, _mqtt{mqtt} {
    _running.store(true);
}

ControlThread::~ControlThread() {
    stop();
}

void ControlThread::run() {
    while (_running) {
        AppState state = _config.snapshot();

        _hvac.loop(state);
        if (_lastState != _hvac.getCurrentState() || state.mode != static_cast<size_t>(_hvac.getCurrentState())) {
            _lastState = _hvac.getCurrentState();
            _mqtt.publish("thermostat/hvac/state", std::to_string(static_cast<int>(_lastState)));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 1Hz
    }
}

void ControlThread::stop() {
    _running.store(false);
}
