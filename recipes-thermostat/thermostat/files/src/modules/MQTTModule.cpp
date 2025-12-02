#include "modules/MQTTModule.h"

MqttModule::MqttModule(const std::string broker, const std::string clientId, int qos)
    : _client(broker, clientId), _qos(qos) {
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

void MqttModule::subscribe(const std::string &topic, MessageCallback callback) {
    std::lock_guard<std::mutex> lock(_callbackMutex);
    _callbacks[topic] = callback;
    try {
        _client.subscribe(topic, _qos, nullptr, *this)->wait();
    } catch (const mqtt::exception &exc) {
        std::cerr << "[MQTT] Error subscribing to topic " << topic << ": " << exc.what() << std::endl;
    }
}

void MqttModule::connection_lost(const std::string &cause) {
    std::cerr << "[MQTT] Connection lost: " << cause << std::endl;
    reconnect();
}

void MqttModule::message_arrived(mqtt::const_message_ptr msg) {
    std::lock_guard<std::mutex> lock(_callbackMutex);
    auto it = _callbacks.find(msg->get_topic());
    if (it != _callbacks.end()) {
        it->second(msg->get_topic(), msg->to_string());
    }
}

void MqttModule::on_failure(const mqtt::token& tok) {
    std::cerr << "[MQTT] Action failed: " << tok.get_message_id() << std::endl;
}

void MqttModule::on_success(const mqtt::token& tok) {
    std::cout << "[MQTT] Action succeeded: " << tok.get_message_id() << std::endl;
}
