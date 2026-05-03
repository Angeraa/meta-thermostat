#include "modules/ui/screenRegistry.h"

static HomeScreen homeScreen;

Screen* const screenTable[static_cast<size_t>(ScreenId::Count)] = {
    &homeScreen
};
