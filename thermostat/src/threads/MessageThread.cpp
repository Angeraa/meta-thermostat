#include "threads/MessageThread.h"
#include <thread>

MessageThread::MessageThread(boost::lockfree::spsc_queue<Message> &queue, ConfigModule &config): _queue{queue}, _config{config} {
    _running.store(true);
}

MessageThread::~MessageThread() {
    stop();
}

void MessageThread::run() {
    Message msg;

    while (_running) {
        bool receivedMessage = false;
        while (_queue.pop(msg)) {
            // For now, all messages go to config module since they are all sensor updates
            _config.update(msg);
            receivedMessage = true;
        }
        if (receivedMessage) _config.syncState();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void MessageThread::stop() {
    _running.store(false);
}
