#pragma once

#include <atomic>
#include "utils/IntentQueue.h"
#include "modules/ConfigModule.h"

class MessageThread {
public:
    MessageThread(IntentQueue& queue, ConfigModule& config);
    ~MessageThread();

    void run();
    void stop();
private:
    IntentQueue& _queue;
    ConfigModule &_config;

    std::atomic<bool> _running;
};