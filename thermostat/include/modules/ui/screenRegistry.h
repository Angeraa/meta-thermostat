#pragma once

#include "modules/ui/screens/Screen.h"
#include "modules/ui/screens/HomeScreen.h"
#include "modules/ui/screens/SettingsScreen.h"
#include "modules/ui/screens/SetpointScreen.h"

extern Screen* const screenTable[static_cast<size_t>(ScreenId::Count)];
