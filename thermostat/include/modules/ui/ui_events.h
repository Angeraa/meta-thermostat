#pragma once

#include "modules/ui/UIEvent.h"

class UIThread;

void registerUIThread(UIThread* ui);
void unregisterUIThread();
void postUIEvent(const UIEvent& e);
