#include "utils/console.h"
#include "modules/MQTTModule.h"
#include "modules/SensorModule.h"
#include "modules/HvacModule.h"
#include <signal.h>

std::atomic<bool> running(true);

void signalHandler(int signum) {
    running = false;
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    banner("Personal Thermostat Application", '=');



    // MqttModule mqttModule("tcp://localhost:1883", "thermostat_client", 1);
    // mqttModule.connect();

    // BME680 bme680("/dev/i2c-1", 0x76);

    // SensorModule sensorModule(mqttModule, bme680);
    // HvacModule hvacModule(mqttModule);

    // sensorModule.start();
    // hvacModule.start();

    // while (running) {
    //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // }

    // sensorModule.stop();
    // bme680.closeBME680();
    // hvacModule.stop();
    // mqttModule.disconnect();

    banner("Shutting down Personal Thermostat Application", '=');

    return 0;
}