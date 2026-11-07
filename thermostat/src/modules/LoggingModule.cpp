#include "modules/LoggingModule.h"
#include <iostream>
#include <string>
#include <chrono>

const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string ORANGE = "\033[38;5;208m";
const std::string RESET = "\033[0m";

static std::string logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return YELLOW + "DEBUG" + RESET;
        case LogLevel::Info: return GREEN + "INFO" + RESET;
        case LogLevel::Warning: return ORANGE + "WARNING" + RESET;
        case LogLevel::Error: return RED + "ERROR" + RESET;
        default: return "UNKNOWN";
    }
}

static std::string formatTimestamp(uint64_t timestamp) {
    std::time_t time = static_cast<std::time_t>(timestamp / 1000);
    std::tm* tm = std::localtime(&time);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
    return std::string(buffer);
}

LoggingModule::LoggingModule(boost::lockfree::queue<Log, boost::lockfree::capacity<32>>* queue) : logQueue_(queue) {
    registerLogQueue(queue);
}

LoggingModule::~LoggingModule() {
    registerLogQueue(nullptr);
}

void LoggingModule::stop() {
    running_.store(false);
}

void LoggingModule::run() {
    while (running_.load()) {
        while (logQueue_ && !logQueue_->empty()) {
            Log logEntry;
            if (logQueue_->pop(logEntry)) {
                // Just print now but later also mqtt to external server perhaps
                std::cout << "[" << formatTimestamp(logEntry.timestamp) << "] "
                          << "[" << logLevelToString(logEntry.level) << "] "
                          << logEntry.caller << ": " << logEntry.message << std::endl;
            }
        }
    }
}