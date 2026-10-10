#pragma once

#include <atomic>
#include "boost/lockfree/spsc_queue.hpp"
#include "utils/messages.h"
#include "modules/ConfigModule.h"

class MessageThread {
public:
    MessageThread(boost::lockfree::spsc_queue<Message> &queue, ConfigModule &config);
    ~MessageThread();

    void run();
    void stop();
private:
    boost::lockfree::spsc_queue<Message> &_queue;
    ConfigModule &_config;

    std::atomic<bool> _running;
};