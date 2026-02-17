#pragma once

#include "lvgl/lvgl.h"
#include "utils/AppState.h"

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
    Count
};

constexpr size_t screenCount = static_cast<size_t>(ScreenId::Count);

constexpr size_t toIndex(ScreenId id) {
    return static_cast<size_t>(id);
}