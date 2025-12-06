#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>
#include <iostream>

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
        initBSEC();
    } catch (...) {
        closeBME680();
        throw;
    }
}

void BME680::closeBME680() {
    if (_i2cFd >= 0) {
        close(_i2cFd);
        _i2cFd = -1;
    }
}

BME680::~BME680() { closeBME680(); }

void BME680::initBME68X() {
    int8_t res = bme68x_init(&_sensorDev);
    if (res != BME68X_OK) {
        throw std::runtime_error("Failed to initialize BME680 sensor");
    }
}

void BME680::initBSEC() {
    bsec_init();

    bsec_sensor_configuration_t sensorConfig[4];
    sensorConfig[0].sensor_id = BSEC_OUTPUT_IAQ;
    sensorConfig[0].sample_rate = BSEC_SAMPLE_RATE_LP;
    sensorConfig[1].sensor_id = BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE;
    sensorConfig[1].sample_rate = BSEC_SAMPLE_RATE_LP;
    sensorConfig[2].sensor_id = BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY;
    sensorConfig[2].sample_rate = BSEC_SAMPLE_RATE_LP;
    sensorConfig[3].sensor_id = BSEC_OUTPUT_RAW_PRESSURE;
    sensorConfig[3].sample_rate = BSEC_SAMPLE_RATE_LP;

    bsec_sensor_configuration_t requiredSensorSettings[BSEC_MAX_PHYSICAL_SENSOR];
    uint8_t nRequiredSensorSettings = BSEC_MAX_PHYSICAL_SENSOR;

    bsec_library_return_t status =
        bsec_update_subscription(sensorConfig, 4, requiredSensorSettings, &nRequiredSensorSettings);
    if (status < BSEC_OK) {
        throw std::runtime_error("Failed to update BSEC subscription, Code: " + std::to_string(status));
    } else if (status > BSEC_OK) {
        std::cout << "Warning during BSEC subscription update, Code: " << status << std::endl;
    }
}

BME680ReadResult BME680::readSensorData(float &temperature, float &humidity, float &pressure, float &iaq,
                                        float &iaqAccuracy) {
    int64_t ts =
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count();

    bsec_bme_settings_t settings{};
    auto status = bsec_sensor_control(ts, &settings);
    if (status < BSEC_OK) {
        throw std::runtime_error("Failed to get BSEC sensor controls, Code: " + std::to_string(status));
        return BME680ReadResult::FAILURE;
    } else if (status > BSEC_OK && status != BSEC_W_SC_CALL_TIMING_VIOLATION) { // Ignore timing violation warnings as they are expected and handled by skipping measurements
        std::cout << "Warning during BSEC sensor control, Code: " << status << std::endl;
    }

    if (settings.trigger_measurement) {
        bme68x_conf conf{};
        conf.filter = BME68X_FILTER_SIZE_7;
        conf.os_temp = settings.temperature_oversampling;
        conf.os_hum = settings.humidity_oversampling;
        conf.os_pres = settings.pressure_oversampling;
        if (bme68x_set_conf(&conf, &_sensorDev) != BME68X_OK) {
            return BME680ReadResult::FAILURE;
        }

        bme68x_heatr_conf heatrConf{};
        if (settings.run_gas) {
            heatrConf.enable = BME68X_ENABLE;
            heatrConf.heatr_temp = settings.heater_temperature;
            heatrConf.heatr_dur = settings.heater_duration;
        } else {
            heatrConf.enable = BME68X_DISABLE;
        }
        if (bme68x_set_heatr_conf(settings.op_mode, &heatrConf, &_sensorDev) != BME68X_OK) {
            return BME680ReadResult::FAILURE;
        }

        if (bme68x_set_op_mode(settings.op_mode, &_sensorDev) != BME68X_OK) {
            return BME680ReadResult::FAILURE;
        }

        int measurementDurationMs = bme68x_get_meas_dur(settings.op_mode, &conf, &_sensorDev) / 1000;
        std::this_thread::sleep_for(
            std::chrono::milliseconds(std::max(measurementDurationMs, static_cast<int>(settings.heater_duration))));

        bme68x_data data;
        uint8_t nFields = 0;

        if (bme68x_get_data(settings.op_mode, &data, &nFields, &_sensorDev) != BME68X_OK || nFields == 0) {
            return BME680ReadResult::FAILURE;
        }

        bsec_input_t inputs[4];
        uint8_t nInputs = 4;
        inputs[0].sensor_id = BSEC_INPUT_GASRESISTOR;
        inputs[0].signal = data.gas_resistance;
        inputs[0].time_stamp = ts;
        inputs[0].signal_dimensions = 1;
        inputs[1].sensor_id = BSEC_INPUT_TEMPERATURE;
        inputs[1].signal = data.temperature;
        inputs[1].time_stamp = ts;
        inputs[1].signal_dimensions = 1;
        inputs[2].sensor_id = BSEC_INPUT_HUMIDITY;
        inputs[2].signal = data.humidity;
        inputs[2].time_stamp = ts;
        inputs[2].signal_dimensions = 1;
        inputs[3].sensor_id = BSEC_INPUT_PRESSURE;
        inputs[3].signal = data.pressure;
        inputs[3].time_stamp = ts;
        inputs[3].signal_dimensions = 1;

        bsec_output_t outputs[4];
        uint8_t nOutputs = 4;

        bsec_library_return_t status = bsec_do_steps(inputs, nInputs, outputs, &nOutputs);
        if (status < BSEC_OK) {
            throw std::runtime_error("Failed to perform BSEC steps, Code: " + std::to_string(status));
            return BME680ReadResult::FAILURE;
        } else if (status > BSEC_OK) {
            std::cout << "Warning during BSEC steps, Code: " << status << std::endl;
        }

        for (uint8_t i = 0; i < nOutputs; ++i) {
            switch (outputs[i].sensor_id) {
            case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE:
                temperature = outputs[i].signal;
                break;
            case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY:
                humidity = outputs[i].signal;
                break;
            case BSEC_OUTPUT_RAW_PRESSURE:
                pressure = outputs[i].signal;
                break;
            case BSEC_OUTPUT_IAQ:
                iaq = outputs[i].signal;
                iaqAccuracy = outputs[i].accuracy;
                break;
            default:
                break;
            }
        }

        return BME680ReadResult::SUCCESS;
    } else {
        return BME680ReadResult::SKIPPED;
    }
}