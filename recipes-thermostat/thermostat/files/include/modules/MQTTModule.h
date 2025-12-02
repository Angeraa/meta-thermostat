#pragma once

#include <mqtt/async_client.h>
#include <string>
#include <functional>
#include <map>
#include <mutex>

class MqttModule : public virtual mqtt::callback, public virtual mqtt::iaction_listener {
public:
    using MessageCallback = std::function<void(const std::string& topic, const std::string& payload)>;

    MqttModule(const std::string broker, const std::string clientId, int qos = 1);
    ~MqttModule();

    void connect();
    void disconnect();
    void publish(const std::string& topic, const std::string& payload);
    void subscribe(const std::string& topic, MessageCallback callback);

private:
    mqtt::async_client _client;
    mqtt::connect_options _connOpts;
    int _qos;
    std::mutex _callbackMutex;
    std::map<std::string, MessageCallback> _callbacks;
    // mqtt::callback overrides
    void connection_lost(const std::string& cause) override;
    void message_arrived(mqtt::const_message_ptr msg) override;

    // mqtt::iaction_listener overrides
    void on_success(const mqtt::token& tok) override;
    void on_failure(const mqtt::token& tok) override;

    void reconnect();
};