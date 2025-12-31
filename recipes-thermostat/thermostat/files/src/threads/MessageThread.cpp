#include "threads/MessageThread.h"
#include <thread>

MessageThread::MessageThread(boost::lockfree::spsc_queue<Message> &queue, ConfigModule &config): _queue{queue}, _config{config} {}

MessageThread::~MessageThread() {
    stop();
}

void MessageThread::run() {
    _running.store(true);
    Message msg;

    while (_running) {
        while (_queue.pop(msg)) {
            // For now, all messages go to config module since they are all sensor updates
            _config.update(msg);
        }
        // Ensure state is synced after processing all messages which is fine because state is small
        // More complex logic is needed later like batching a specific number, time, specific type of messages, etc. when it eventually grows
        _config.syncState();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void MessageThread::stop() {
    _running.store(false);
}
