#pragma once

#include <string>
#include <sstream>
#include <iomanip>

static std::string formatTemperature(int tenthsOfDegree) {
    std::ostringstream value;
    value << std::fixed << std::setprecision(1)
          << (static_cast<float>(tenthsOfDegree) / 10.0f);
    return value.str();
}

static std::string formatHumidity(int humidity) {
    return std::to_string(humidity) + "% r.H.";
}

static std::string formatPressure(int pressure) {
    std::ostringstream value;
    value << std::fixed << std::setprecision(0)
          << (static_cast<float>(pressure) / 100.0f);
    return value.str() + " kPa";
}