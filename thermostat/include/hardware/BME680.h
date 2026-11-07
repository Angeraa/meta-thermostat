#pragma once

#include "bme68x.h"
#include "bsec_interface.h"


enum class BME680ReadResult {
    SUCCESS,
    FAILURE,
    SKIPPED
};
class BME680 {
public:
    BME680(const char *i2cDevice, uint8_t i2cAddress);
    ~BME680();

    // _sensorDev.intf_ptr points at this instance's _i2cFd, so copies would dangle.
    BME680(const BME680&) = delete;
    BME680& operator=(const BME680&) = delete;

    BME680ReadResult readSensorData(float &temperature, float &humidity, float &pressure, float &iaq, float &iaqAccuracy);
    void closeBME680();

private:
    void initBME68X();
    void initBSEC();

    int _i2cFd = -1;
    bme68x_dev _sensorDev{};
};