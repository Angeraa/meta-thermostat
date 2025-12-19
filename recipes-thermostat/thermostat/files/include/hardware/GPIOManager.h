#pragma once

#include <gpiod.hpp>
#include <string>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <optional>

enum class GPIOResult {
    SUCCESS,
    ERROR_OPENING_CHIP,
    ERROR_SETTING_PIN_VALUE,
    ERROR_GETTING_PIN_VALUE,
    ERROR_REQUESTING_LINE,
    ERROR_INITIALIZATION,
    ERROR_PIN_ALREADY_REQUESTED,
    ERROR_PIN_NOT_REQUESTED,
    ERROR_PIN_DOES_NOT_EXIST,
    ERROR_NOT_OWNER
};

class GPIOManager {
public:
    static GPIOManager& getInstance();

    GPIOResult requestPin(int pinNumber, const std::string &owner, bool isOutput);
    GPIOResult releasePin(int pinNumber, const std::string &owner);
    GPIOResult setPinValue(int pinNumber, bool value);
    GPIOResult getPinValue(int pinNumber, bool &value);
private:
    GPIOManager();
    ~GPIOManager();

    void discoverChipsAndLines();

    std::mutex _mapMutex;

    struct LineRef {
        std::shared_ptr<gpiod::chip> chip;
        std::optional<gpiod::line_request> lineRequest;
        std::string owner;
    };
    std::unordered_map<int, LineRef> pinMapping;
    std::unordered_map<int, std::mutex> pinMutexes;

};