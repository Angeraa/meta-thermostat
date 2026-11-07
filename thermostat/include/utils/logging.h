#pragma once

#include <string>
#include <cstdint>
#include "boost/lockfree/queue.hpp"

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

struct Log {
    char message[256];
    char caller[64];
    uint64_t timestamp;
    LogLevel level;
};

void registerLogQueue(boost::lockfree::queue<Log, boost::lockfree::capacity<32>>* queue);

void logDebug(const std::string& message, const std::string& caller);

void logInfo(const std::string& message, const std::string& caller);

void logWarning(const std::string& message, const std::string& caller);

void logError(const std::string& message, const std::string& caller);