#include "hardware/GPIOManager.h"

GPIOManager &GPIOManager::getInstance() {
    static GPIOManager instance;
    return instance;
}

GPIOManager::GPIOManager() { discoverChipsAndLines(); }

GPIOManager::~GPIOManager() {
    // Release all requested pins
    for (auto &entry : pinMapping) {
        if (entry.second.lineRequest) {
            entry.second.lineRequest.value().release();
        }
    }
}

void GPIOManager::discoverChipsAndLines() {
    // The Pi has all the GPIO lines labeled with numbers on chip 0 so I'll use that one
    auto chip = std::make_shared<gpiod::chip>("/dev/gpiochip0");
    int num_lines = chip->get_info().num_lines();
    for (int i = 0; i < num_lines; ++i) {
        pinMutexes[i];
        pinMapping[i] = LineRef{chip, std::nullopt, ""};
    }
}

GPIOResult GPIOManager::requestPin(int pinNumber, const std::string &owner, bool isOutput) {
    std::lock_guard<std::mutex> mapLock(_mapMutex);
    auto it = pinMapping.find(pinNumber);
    if (it == pinMapping.end()) {
        return GPIOResult::ERROR_PIN_DOES_NOT_EXIST;
    }

    LineRef &lineRef = it->second;
    if (lineRef.lineRequest.has_value()) {
        return GPIOResult::ERROR_PIN_ALREADY_REQUESTED;
    }

    try {
        auto req = lineRef.chip->prepare_request()
                       .add_line_settings(
                           pinNumber, gpiod::line_settings().set_direction(isOutput ? gpiod::line::direction::OUTPUT
                                                                                    : gpiod::line::direction::INPUT))
                       .do_request();
        lineRef.lineRequest = std::move(req);
        lineRef.owner = owner;
    } catch (const std::exception &e) {
        return GPIOResult::ERROR_REQUESTING_LINE;
    }

    return GPIOResult::SUCCESS;
}

GPIOResult GPIOManager::releasePin(int pinNumber, const std::string &owner) {
    std::lock_guard<std::mutex> mapLock(_mapMutex);
    auto it = pinMapping.find(pinNumber);
    if (it == pinMapping.end()) {
        return GPIOResult::ERROR_PIN_DOES_NOT_EXIST;
    }

    std::lock_guard<std::mutex> pinLock(pinMutexes[pinNumber]);
    LineRef &lineRef = it->second;
    if (!lineRef.lineRequest.has_value()) {
        return GPIOResult::ERROR_PIN_NOT_REQUESTED;
    }

    if (lineRef.owner != owner) {
        return GPIOResult::ERROR_NOT_OWNER;
    }

    lineRef.lineRequest->release();
    lineRef.lineRequest = std::nullopt;
    lineRef.owner = "";

    return GPIOResult::SUCCESS;
}

GPIOResult GPIOManager::setPinValue(int pinNumber, bool value) {
    std::lock_guard<std::mutex> mapLock(_mapMutex);
    auto it = pinMapping.find(pinNumber);
    if (it == pinMapping.end()) {
        return GPIOResult::ERROR_PIN_DOES_NOT_EXIST;
    }

    std::lock_guard<std::mutex> pinLock(pinMutexes[pinNumber]);
    LineRef &lineRef = it->second;
    if (!lineRef.lineRequest.has_value()) {
        return GPIOResult::ERROR_PIN_NOT_REQUESTED;
    }

    try {
        lineRef.lineRequest->set_value(pinNumber, value ? gpiod::line::value::ACTIVE : gpiod::line::value::INACTIVE);
    } catch (const std::exception &e) {
        return GPIOResult::ERROR_SETTING_PIN_VALUE;
    }

    return GPIOResult::SUCCESS;
}

GPIOResult GPIOManager::getPinValue(int pinNumber, bool &value) {
    std::lock_guard<std::mutex> mapLock(_mapMutex);
    auto it = pinMapping.find(pinNumber);
    if (it == pinMapping.end()) {
        return GPIOResult::ERROR_PIN_DOES_NOT_EXIST;
    }

    std::lock_guard<std::mutex> pinLock(pinMutexes[pinNumber]);
    LineRef &lineRef = it->second;
    if (!lineRef.lineRequest.has_value()) {
        return GPIOResult::ERROR_PIN_NOT_REQUESTED;
    }

    try {
        auto lineValue = lineRef.lineRequest->get_value(pinNumber);
        value = (lineValue == gpiod::line::value::ACTIVE);
    } catch (const std::exception &e) {
        return GPIOResult::ERROR_GETTING_PIN_VALUE;
    }

    return GPIOResult::SUCCESS;
}
