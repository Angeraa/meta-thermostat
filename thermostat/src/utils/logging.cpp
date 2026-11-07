#include "utils/logging.h"
#include <chrono>
#include <cstring>
#include <string>

static boost::lockfree::queue<Log, boost::lockfree::capacity<32>>* logQueue = nullptr;

void registerLogQueue(boost::lockfree::queue<Log, boost::lockfree::capacity<32>>* queue) {
    logQueue = queue;
}

static void log(LogLevel level, const char* message, const char* caller) {
    if (logQueue) {
        Log logEntry;
        std::strncpy(logEntry.message, message, sizeof(logEntry.message) - 1);
        logEntry.message[sizeof(logEntry.message) - 1] = '\0'; // Ensure null termination
        std::strncpy(logEntry.caller, caller, sizeof(logEntry.caller) - 1);
        logEntry.caller[sizeof(logEntry.caller) - 1] = '\0'; // Ensure null termination
        logEntry.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        logEntry.level = level;
        logQueue->push(logEntry);
    }
}

void logDebug(const std::string& message, const std::string& caller) {
    log(LogLevel::Debug, message.c_str(), caller.c_str());
}

void logInfo(const std::string& message, const std::string& caller) {
    log(LogLevel::Info, message.c_str(), caller.c_str());
}

void logWarning(const std::string& message, const std::string& caller) {
    log(LogLevel::Warning, message.c_str(), caller.c_str());
}

void logError(const std::string& message, const std::string& caller) {
    log(LogLevel::Error, message.c_str(), caller.c_str());
}