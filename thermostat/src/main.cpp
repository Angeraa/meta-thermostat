#include "utils/console.h"
#include "modules/MQTTModule.h"
#include "modules/SensorModule.h"
#include "modules/HvacModule.h"
#include "modules/ConfigModule.h"
#include "modules/LoggingModule.h"
#include "utils/logging.h"
#include "threads/ControlThread.h"
#include "threads/MessageThread.h"
#include "threads/UIThread.h"
#include "modules/ui/ui_events.h"
#include <memory>
#include <signal.h>
#include <thread>

std::atomic<bool> running(true);

void signalHandler(int signum) {
    running = false;
}

void enableDebugLoggingHandler(int) {
    setDebugLoggingEnabled(true);
}

void disableDebugLoggingHandler(int) {
    setDebugLoggingEnabled(false);
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGUSR1, enableDebugLoggingHandler);
    signal(SIGUSR2, disableDebugLoggingHandler);

    banner("Personal Thermostat Application", '=');

    // Init messaging objects
    boost::lockfree::spsc_queue<Message> messageQueue(128);
    boost::lockfree::queue<Log, boost::lockfree::capacity<32>> logQueue;
    LoggingModule loggingModule(&logQueue);
    // Start logging module early
    std::thread loggingThread(&LoggingModule::run, &loggingModule);

    ConfigModule configModule = {};

    ScreenManager screenManager = {};
    UIThread uiThread(configModule, screenManager);

    MessageThread messageThread(messageQueue, configModule);
    MqttModule mqttModule("tcp://localhost:1883", "thermostat_client", messageQueue, 1);

    // Init control objects
    HvacModule hvacModule;
    ControlThread controlThread(hvacModule, configModule, mqttModule);

    // Init data objects
    std::unique_ptr<BME680> bme680;
    try {
        bme680 = std::make_unique<BME680>("/dev/i2c-1", 0x76);
    } catch (const std::exception& error) {
        logError("BME680 initialization failed: " + std::string(error.what()), "Startup");
        loggingModule.stop();
        if (loggingThread.joinable()) loggingThread.join();
        return 1;
    }
    SensorModule sensorModule(mqttModule, *bme680);

    registerUIThread(&uiThread);
    auto uiReady = uiThread.ready();
    std::thread uiThreadHandle(&UIThread::run, &uiThread);
    try {
        uiReady.get();
    } catch (const std::exception& error) {
        logError(error.what(), "Startup");
        unregisterUIThread();
        uiThreadHandle.join();
        loggingModule.stop();
        if (loggingThread.joinable()) loggingThread.join();
        return 1;
    }

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
    uiThread.stop();
    unregisterUIThread();
    if (uiThreadHandle.joinable()) uiThreadHandle.join();
    controlThread.stop();
    if (controlThreadHandle.joinable()) controlThreadHandle.join();
    sensorModule.stop();
    bme680->closeBME680();
    mqttModule.disconnect();
    messageThread.stop();
    if (messageThreadHandle.joinable()) messageThreadHandle.join();
    loggingModule.stop();
    if (loggingThread.joinable()) loggingThread.join();

    banner("Shutting down Personal Thermostat Application", '=');
    return 0;
}