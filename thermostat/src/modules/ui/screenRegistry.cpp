#include "modules/ui/screenRegistry.h"

static HomeScreen homeScreen;
static SettingsScreen settingsScreen;
static SetpointScreen setpointScreen;

Screen* const screenTable[static_cast<size_t>(ScreenId::Count)] = {
    &homeScreen,
    &settingsScreen,
    &setpointScreen
};

ScreenId screens_that_need_global_topbar[] = {
    ScreenId::Settings,
    // ScreenId::Sensors,
    // ScreenId::AirQuality,
    // ScreenId::Settings, current Settings changes to Menu
};
