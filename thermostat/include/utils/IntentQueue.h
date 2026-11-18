#pragma once

#include "boost/lockfree/queue.hpp"
#include "utils/messages.h"
#include <cstddef>

constexpr size_t INTENT_QUEUE_CAPACITY = 128;

class MessageThread;

class IntentQueue {
public:
    explicit IntentQueue() = default;

    bool publish(const Message& message) {
        return _queue.push(message);
    }

private:
    friend class MessageThread;

    bool consume(Message& message) {
        return _queue.pop(message);
    }
 
    boost::lockfree::queue<Message, boost::lockfree::capacity<128>> _queue;
};
