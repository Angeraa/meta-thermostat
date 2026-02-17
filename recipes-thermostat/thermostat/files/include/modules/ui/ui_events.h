#pragma once

#include "threads/UIThread.h"

void registerUIThread(UIThread* ui);
void postUIEvent(const UIEvent& e);
