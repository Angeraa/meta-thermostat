#pragma once

#include <mqtt/async_client.h>
#include <boost/lockfree/spsc_queue.hpp>
#include <string>
#include "utils/messages.h"

class MqttModule : public virtual mqtt::callback, public virtual mqtt::iaction_listener {
public:

    MqttModule(const std::string broker, const std::string clientId, boost::lockfree::spsc_queue<Message>& queue, int qos = 1);
    ~MqttModule();

    void connect();
    void disconnect();
    void publish(const std::string& topic, const std::string& payload);

private:
    mqtt::async_client _client;
    mqtt::connect_options _connOpts;
    int _qos;
    boost::lockfree::spsc_queue<Message>& _queue;
    // mqtt::callback overrides
    void connection_lost(const std::string& cause) override;
    void message_arrived(mqtt::const_message_ptr msg) override;

    // mqtt::iaction_listener overrides
    void on_success(const mqtt::token& tok) override;
    void on_failure(const mqtt::token& tok) override;

    void reconnect();
};