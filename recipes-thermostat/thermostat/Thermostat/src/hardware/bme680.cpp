#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>

#include "hardware/bme680.h"

BME680::BME680(const char *i2cDevice, uint8_t i2cAddress) {
    _i2cFd = open(i2cDevice, O_RDWR);
    if (_i2cFd < 0) {
        perror("Failed to open the i2c bus");
        throw std::runtime_error("Failed to open the i2c bus");
    }

    if (ioctl(_i2cFd, I2C_SLAVE, i2cAddress) < 0) {
        perror("Failed to acquire bus access and/or talk to slave");
        close(_i2cFd);
        throw std::runtime_error("Failed to acquire bus access and/or talk to slave");
    }
    _sensorDev.intf = BME68X_I2C_INTF;
    _sensorDev.read = [](uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) -> int8_t {
        int fd = *static_cast<int *>(intf_ptr);
        if (write(fd, &reg_addr, 1) != 1) {
            return BME68X_E_COM_FAIL;
        }
        if (read(fd, reg_data, len) != static_cast<ssize_t>(len)) {
            return BME68X_E_COM_FAIL;
        }
        return 0;
    };
    _sensorDev.write = [](uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) -> int8_t {
        int fd = *static_cast<int *>(intf_ptr);
        uint8_t buffer[len + 1];
        buffer[0] = reg_addr;
        std::memcpy(&buffer[1], reg_data, len);
        if (write(fd, buffer, len + 1) != static_cast<ssize_t>(len + 1)) {
            return BME68X_E_COM_FAIL;
        }
        return 0;
    };
    _sensorDev.delay_us = [](uint32_t period, void *intf_ptr) {
        std::this_thread::sleep_for(std::chrono::microseconds(period));
    };
    _sensorDev.intf_ptr = &_i2cFd;

    try {
        initBME68X();
    } catch (...) {
        close(_i2cFd);
        throw;
    }
}

BME680::~BME680() {
    if (_i2cFd >= 0) {
        close(_i2cFd);
    }
}

void BME680::initBME68X() {
    int8_t res = bme68x_init(&_sensorDev);
    if (res != BME68X_OK) {
        throw std::runtime_error("Failed to initialize BME680 sensor");
    }

    bme68x_conf conf{};
    bme68x_heatr_conf heatrConf{};

    conf.filter = BME68X_FILTER_SIZE_7;
    conf.odr = BME68X_ODR_NONE;
    conf.os_hum = BME68X_OS_4X;
    conf.os_pres = BME68X_OS_2X;
    conf.os_temp = BME68X_OS_8X;

    heatrConf.enable = BME68X_ENABLE;
    heatrConf.heatr_temp = 300; // 300 degree Celsius
    heatrConf.heatr_dur = 100;  // 100 ms

    res = bme68x_set_conf(&conf, &_sensorDev);
    if (res != BME68X_OK) {
        throw std::runtime_error("Failed to set BME680 configuration");
    }

    res = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatrConf, &_sensorDev);
    if (res != BME68X_OK) {
        throw std::runtime_error("Failed to set BME680 heater configuration");
    }

    measurementDurationMs = bme68x_get_meas_dur(BME68X_FORCED_MODE, &conf, &_sensorDev) / 1000;
}

bool BME680::readSensorData(float &temperature, float &humidity, float &pressure, float &gasResistance) {
    bme68x_data data;
    uint8_t nFields = 0;

    if (bme68x_set_op_mode(BME68X_FORCED_MODE, &_sensorDev) != BME68X_OK) {
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(measurementDurationMs));

    if (bme68x_get_data(BME68X_FORCED_MODE, &data, &nFields, &_sensorDev) != BME68X_OK || nFields == 0) {
        return false;
    }

    temperature = data.temperature;
    humidity = data.humidity;
    pressure = data.pressure;
    gasResistance = data.gas_resistance;

    return true;
}