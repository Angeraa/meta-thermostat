#pragma once

#include "lvgl/lvgl.h"
#include "utils/AppState.h"

constexpr int32_t SCREEN_WIDTH = 720;
constexpr int32_t SCREEN_HEIGHT = 1280;
constexpr uint32_t COLOR_BACKGROUND = 0xFF000000;
constexpr uint32_t COLOR_TEXT_MAIN = 0xFFFFFFFF;
constexpr uint32_t COLOR_COOLING = 0xFF2E7EFF;
constexpr uint32_t COLOR_HEATING = 0xFFC26D1E;

class Screen {
public:
    virtual ~Screen() = default;
    virtual void create(lv_obj_t *root) = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void update(const AppState& appState) = 0;
    bool isCreated() {return created;}
protected:
    lv_obj_t* container = nullptr;
    bool created = false;
};

enum class ScreenId : size_t {
    Home = 0,
    Settings,
    Setpoint,
    // Sensors,
    // AirQuality,
    // Settings, current Settings changes to Menu
    Count
};

constexpr size_t screenCount = static_cast<size_t>(ScreenId::Count);

constexpr size_t toIndex(ScreenId id) {
    return static_cast<size_t>(id);
}