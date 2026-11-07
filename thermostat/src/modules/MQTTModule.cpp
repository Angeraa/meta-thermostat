#include "modules/MQTTModule.h"
#include "utils/logging.h"

MqttModule::MqttModule(const std::string broker, const std::string clientId, boost::lockfree::spsc_queue<Message>& queue, int qos)
    : _client(broker, clientId), _queue(queue), _qos(qos) {
    _connOpts.set_clean_session(true);
    _client.set_callback(*this);
}

MqttModule::~MqttModule() {
    disconnect();
}

void MqttModule::connect() {
    try {
        _client.connect(_connOpts, nullptr, *this)->wait();
    } catch (const mqtt::exception &exc) {
        logError("Error connecting to the MQTT broker: " + std::string(exc.what()), "MQTT");
        reconnect();
    }
}

void MqttModule::disconnect() {
    try {
        _client.disconnect()->wait();
    } catch (const mqtt::exception &exc) {
        logError("Error disconnecting from the MQTT broker: " + std::string(exc.what()), "MQTT");
    }
}

void MqttModule::connected(const std::string &cause) {
    logInfo("Connected: " + cause, "MQTT");
    logInfo("Subscribing to all topics...", "MQTT");
    for (const auto &e : messageEntries) {
        _client.subscribe(e.key, _qos, nullptr, *this);
    }
}

void MqttModule::reconnect() {
    int tries = 0;
    const int maxTries = 5;
    while (tries < maxTries) {
        try {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            _client.connect(_connOpts, nullptr, *this)->wait();
            return;
        } catch (const mqtt::exception &exc) {
            logWarning("Reconnect attempt " + std::to_string(++tries) + " failed: " + exc.what(), "MQTT");
        }
    }
}

void MqttModule::publish(const std::string &topic, const std::string &payload) {
    try {
        logDebug("Publishing to topic: " + topic + " payload: " + payload, "MQTT");
        _client.publish(topic, payload.c_str(), payload.size(), _qos, false);
    } catch (const mqtt::exception &exc) {
        logError("Error publishing message: " + std::string(exc.what()), "MQTT");
    }
}

void MqttModule::connection_lost(const std::string &cause) {
    logError("Connection lost: " + cause, "MQTT");
    reconnect();
}

void MqttModule::message_arrived(mqtt::const_message_ptr msg) {
    for (const auto &e : messageEntries) {
        logDebug("Checking topic: " + msg->get_topic() + " against entry key: " + e.key, "MQTT");
        if (msg->get_topic() == e.key) {
            Message message;
            message.type = e.meta.first;
            try {
                std::string data = msg->to_string();
                switch (e.meta.second) {
                    case PayloadType::Int:
                        message.data = std::stoi(data);
                        break;
                    case PayloadType::Float:
                        message.data = std::stof(data);
                        break;
                    case PayloadType::String:
                        message.data = data;
                        break;
                    case PayloadType::SizeT:
                        message.data = std::stoul(data);
                        break;
                }
                logDebug("Received message on topic: " + msg->get_topic() + " with payload: " + data, "MQTT");
                _queue.push(message);
                return;
            } catch (const std::exception &exc) {
                logError("Error parsing message payload: " + std::string(exc.what()), "MQTT");
            }
            break;
        }
    }
    logError("Received message on unknown topic: " + msg->get_topic(), "MQTT");
}

void MqttModule::on_failure(const mqtt::token& tok) {
    logError("Action failed: " + std::to_string(tok.get_message_id()), "MQTT");
}

void MqttModule::on_success(const mqtt::token& tok) {
    logInfo("Action succeeded: " + std::to_string(tok.get_message_id()), "MQTT");
}
