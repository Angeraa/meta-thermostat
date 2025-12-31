#include "utils/console.h"
#include "modules/MQTTModule.h"
#include "modules/SensorModule.h"
#include "modules/HvacModule.h"
#include "modules/ConfigModule.h"
#include "threads/ControlThread.h"
#include "threads/MessageThread.h"
#include <signal.h>
#include <thread>

std::atomic<bool> running(true);

void signalHandler(int signum) {
    running = false;
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    banner("Personal Thermostat Application", '=');

    // Init messaging objects
    boost::lockfree::spsc_queue<Message> messageQueue(128);
    ConfigModule configModule = {};
    MessageThread messageThread(messageQueue, configModule);
    MqttModule mqttModule("tcp://localhost:1883", "thermostat_client", messageQueue, 1);

    // Init control objects
    HvacModule hvacModule;
    ControlThread controlThread(hvacModule, configModule);

    // Init data objects
    BME680 bme680("/dev/i2c-1", 0x76);
    SensorModule sensorModule(mqttModule, bme680);

    // Start messaging system and data acquisition
    std::thread messageThreadHandle(&MessageThread::run, &messageThread);
    mqttModule.connect();
    sensorModule.start();

    // Start control
    std::thread controlThreadHandle(&ControlThread::run, &controlThread);

    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Shutdown objects in oposite order
    if (controlThreadHandle.joinable()) controlThreadHandle.join();
    sensorModule.stop();
    bme680.closeBME680();
    mqttModule.disconnect();
    if (messageThreadHandle.joinable()) messageThreadHandle.join();

    banner("Shutting down Personal Thermostat Application", '=');
    return 0;
}