#pragma once

#include "boost/lockfree/queue.hpp"
#include "utils/logging.h"
#include <atomic>

class LoggingModule {
public:
    LoggingModule(boost::lockfree::queue<Log, boost::lockfree::capacity<32>>* queue);
    ~LoggingModule();

    void run();
    void stop();

private:
    std::atomic<bool> running_{true};
    boost::lockfree::queue<Log, boost::lockfree::capacity<32>>* logQueue_;
};