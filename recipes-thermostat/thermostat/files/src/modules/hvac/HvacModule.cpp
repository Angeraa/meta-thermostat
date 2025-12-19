#include "modules/hvac/HvacModule.h"

HvacModule::HvacModule(MqttModule& mqttModule) : _mqttModule(mqttModule) {}

void HvacModule::start() {
    std::cout << "[HVAC] Starting HVAC Module..." << std::endl;
    _mqttModule.subscribe("sensor/temperature", [this](const std::string& topic, const std::string& payload) {
        std::cout << "[HVAC] Received message on topic: " << topic << " with payload: " << payload << std::endl;
        enqueueMessage(topic, payload);
    });
    _running = true;

    // Start worker thread to process messages
    _workerThread = std::thread(&HvacModule::processMessages, this);

    // Start control loop thread for the HvacController object
    _controlThread = std::thread(&HvacModule::controlLoop, this);
}

void HvacModule::stop() {
    std::cout << "[HVAC] Stopping HVAC Module..." << std::endl;
    _running = false;
    _cv.notify_all();
    if (_workerThread.joinable()) {
        _workerThread.join();
    }
    if (_controlThread.joinable()) {
        _controlThread.join();
    }
}

HvacModule::~HvacModule() noexcept {
    stop();
}

void HvacModule::enqueueMessage(const std::string& topic, const std::string& payload) {
    {
        std::lock_guard<std::mutex> lock(_queueMutex);
        if (_messageQueue.size() >= 20) {
            std::cerr << "[HVAC] Message queue full, dropping oldest message" << std::endl;
            _messageQueue.pop();
        }
        _messageQueue.push({topic, payload});
    }
    _cv.notify_one();
}

void HvacModule::processMessages() {
    while (_running) {
        Message msg;
        {
            std::unique_lock<std::mutex> lock(_queueMutex);
            _cv.wait(lock, [this]() { return !_messageQueue.empty() || !_running; });
            if (!_running && _messageQueue.empty()) {
                break;
            }
            msg = _messageQueue.front();
            _messageQueue.pop();
        }
        handleMessage(msg);
    }
}

void HvacModule::handleMessage(const Message& msg) {
    // Process the message and update HVAC state accordingly
    std::lock_guard<std::mutex> lock(_stateMutex);
    std::cout << "[HVAC] Handling message on topic: " << msg.topic << " with payload: " << msg.payload << std::endl;
    if (msg.topic == "sensor/temperature") {
        _hvacController.setCurrentTemp(std::stoi(msg.payload));
    }
}

void HvacModule::controlLoop() {
    while (_running) {
        {
            std::lock_guard<std::mutex> lock(_stateMutex);
            std::cout << "[HVAC] Running control loop..." << std::endl;
            _hvacController.loop(); // Call the control loop of the HVAC controller, handles actual HVAC logic with an internal state machine
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Control loop interval, 2hz for now maybe faster later
    }
}
