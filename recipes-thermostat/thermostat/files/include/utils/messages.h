#pragma once

#include <string>
#include <variant>

enum class MessageType {
    SensorTemp,
    SensorHum,
    SensorPres,
    SensorIAQ,
    SensorIAQAcc,
};

// In the future, there might be another message specifically for the MQTT broker that is more general as config updates
// are very specific in nature but for now they can be the same so it will be given a general name like Message
struct Message {
    MessageType type;
    std::variant<int, float, std::string> data;
};