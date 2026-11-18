#pragma once

#include <string>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <utility>

enum class MessageType {
    SensorTemp,
    SensorHum,
    SensorPres,
    SensorIAQ,
    SensorIAQAcc,
    HVACState,
    HeatSetpoint,
    CoolSetpoint,
    HVACMode
};

enum class PayloadType {
    Int,
    Float,
    String,
    SizeT
};

struct Entry {
    const char* key;
    std::pair<MessageType, PayloadType> meta;
};

constexpr Entry messageEntries[] = {
    {"sensor/temp", {MessageType::SensorTemp, PayloadType::Int}},
    {"sensor/hum", {MessageType::SensorHum, PayloadType::Int}},
    {"sensor/pres", {MessageType::SensorPres, PayloadType::Float}},
    {"sensor/iaq", {MessageType::SensorIAQ, PayloadType::Float}},
    {"sensor/iaq_acc", {MessageType::SensorIAQAcc, PayloadType::Float}},
    {"thermostat/hvac/state", {MessageType::HVACState, PayloadType::SizeT}},
    {"thermostat/heat_setpoint", {MessageType::HeatSetpoint, PayloadType::Int}},
    {"thermostat/cool_setpoint", {MessageType::CoolSetpoint, PayloadType::Int}},
    {"thermostat/hvac/mode", {MessageType::HVACMode, PayloadType::SizeT}}
};

union MessagePayload {
    int intValue;
    float floatValue;
    char stringValue[128];
    size_t sizeValue;
};

struct Message {
    MessageType type = MessageType::SensorTemp;
    PayloadType payload = PayloadType::Int;
    MessagePayload data{};

    Message() = default;

    Message(MessageType messageType, int value)
        : type(messageType), payload(PayloadType::Int) {
        data.intValue = value;
    }

    Message(MessageType messageType, float value)
        : type(messageType), payload(PayloadType::Float) {
        data.floatValue = value;
    }

    Message(MessageType messageType, const std::string& value)
        : type(messageType), payload(PayloadType::String) {
        const size_t length = value.copy(data.stringValue,
                                         sizeof(data.stringValue) - 1);
        data.stringValue[length] = '\0';
    }

    Message(MessageType messageType, const char* value)
        : type(messageType), payload(PayloadType::String) {
        const size_t length = value ? std::strlen(value) : 0;
        const size_t copyLength = length < sizeof(data.stringValue)
                                      ? length
                                      : sizeof(data.stringValue) - 1;
        if (value) {
            std::memcpy(data.stringValue, value, copyLength);
        }
        data.stringValue[copyLength] = '\0';
    }

    Message(MessageType messageType, size_t value)
        : type(messageType), payload(PayloadType::SizeT) {
        data.sizeValue = value;
    }
};

static_assert(std::is_trivially_copyable_v<MessagePayload>);
static_assert(std::is_trivially_copyable_v<Message>);