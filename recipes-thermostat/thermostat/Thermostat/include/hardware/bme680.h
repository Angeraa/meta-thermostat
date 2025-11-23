#include "bme68x/bme68x.h"

class BME680 {
public:
    BME680(const char *i2cDevice, uint8_t i2cAddress);
    ~BME680();

    bool readSensorData(float &temperature, float &humidity, float &pressure, float &gasResistance);

private:
    void initBME68X();

    int _i2cFd;
    int measurementDurationMs;
    bme68x_dev _sensorDev;
};