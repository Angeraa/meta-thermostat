#pragma once

#include "modules/MQTTModule.h"
#include <queue>
#include <string>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>

class HvacModule {
public:
    HvacModule(MqttModule& mqttModule);
    ~HvacModule() noexcept;
    void start();
    void stop();

private:
    struct Message {
        std::string topic;
        std::string payload;
    };

    MqttModule& _mqttModule;
    std::queue<Message> _messageQueue;
    std::mutex _queueMutex;
    std::mutex _stateMutex;
    std::condition_variable _cv;
    std::thread _workerThread;
    std::thread _controlThread;
    std::atomic<bool> _running;

    void enqueueMessage(const std::string& topic, const std::string& payload);
    void processMessages();
    void handleMessage(const Message& msg);
    void controlLoop();

    // HvacController hvacController_; This handles actual HVAC logic 
};