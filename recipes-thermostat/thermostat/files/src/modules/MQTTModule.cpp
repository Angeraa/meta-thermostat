#include "modules/MQTTModule.h"

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
        std::cerr << "[MQTT] Error connecting to the MQTT broker: " << exc.what() << std::endl;
        reconnect();
    }
}

void MqttModule::disconnect() {
    try {
        _client.disconnect()->wait();
    } catch (const mqtt::exception &exc) {
        std::cerr << "[MQTT] Error disconnecting from the MQTT broker: " << exc.what() << std::endl;
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
            std::cerr << "[MQTT] Reconnect attempt " << ++tries << " failed: " << exc.what() << std::endl;
        }
    }
}

void MqttModule::publish(const std::string &topic, const std::string &payload) {
    try {
        _client.publish(topic, payload.c_str(), payload.size(), _qos, false);
    } catch (const mqtt::exception &exc) {
        std::cerr << "[MQTT] Error publishing message: " << exc.what() << std::endl;
    }
}

void MqttModule::connection_lost(const std::string &cause) {
    std::cerr << "[MQTT] Connection lost: " << cause << std::endl;
    reconnect();
}

void MqttModule::message_arrived(mqtt::const_message_ptr msg) {
    for (const auto &e : messageEntries) {
        if (msg->get_topic() == e.key) {
            Message message;
            message.type = e.meta.first;
            try {
                switch (e.meta.second) {
                    case PayloadType::Int:
                        message.data = std::stoi(msg->to_string());
                        break;
                    case PayloadType::Float:
                        message.data = std::stof(msg->to_string());
                        break;
                    case PayloadType::String:
                        message.data = msg->to_string();
                        break;
                }
                _queue.push(message);
                return;
            } catch (const std::exception &exc) {
                std::cout << "[MQTT] Error parsing message payload: " << exc.what() << std::endl;
            }
            break;
        }
    }
    std::cout << "[MQTT] Critical: Received message on unknown topic: " << msg->get_topic() << std::endl;
}

void MqttModule::on_failure(const mqtt::token& tok) {
    std::cerr << "[MQTT] Action failed: " << tok.get_message_id() << std::endl;
}

void MqttModule::on_success(const mqtt::token& tok) {
    std::cout << "[MQTT] Action succeeded: " << tok.get_message_id() << std::endl;
}
