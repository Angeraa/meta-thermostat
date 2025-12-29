#pragma once

#include <string>
#include <variant>
#include <utility>

enum class MessageType {
    SensorTemp,
    SensorHum,
    SensorPres,
    SensorIAQ,
    SensorIAQAcc,
};

enum class PayloadType {
    Int,
    Float,
    String
};

struct Entry {
    std::string_view key;
    std::pair<MessageType, PayloadType> meta;
};

constexpr Entry messageEntries[] = {
    {"sensor/temp", {MessageType::SensorTemp, PayloadType::Int}},
    {"sensor/hum", {MessageType::SensorHum, PayloadType::Int}},
    {"sensor/pres", {MessageType::SensorPres, PayloadType::Float}},
    {"sensor/iaq", {MessageType::SensorIAQ, PayloadType::Float}},
    {"sensor/iaq_acc", {MessageType::SensorIAQAcc, PayloadType::Float}}
};

// In the future, there might be another message specifically for the MQTT broker that is more general as config updates
// are very specific in nature but for now they can be the same so it will be given a general name like Message
struct Message {
    MessageType type;
    std::variant<int, float, std::string> data;
};