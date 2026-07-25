#include "modules/ui/screenRegistry.h"

static HomeScreen homeScreen;
static SettingsScreen settingsScreen;

Screen* const screenTable[static_cast<size_t>(ScreenId::Count)] = {
    &homeScreen,
    &settingsScreen
};
