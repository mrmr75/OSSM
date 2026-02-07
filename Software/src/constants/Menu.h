#ifndef OSSM_SOFTWARE_MENU_H
#define OSSM_SOFTWARE_MENU_H

#include <Arduino.h>

#include "constants/UserConfig.h"

enum Menu {
    SimplePenetration,
    StrokeEngine,
    Streaming,
    Help,
    Restart,
    NUM_OPTIONS
};

static const char* menuStrings[Menu::NUM_OPTIONS] = {
    UserConfig::language.SimplePenetration, UserConfig::language.StrokeEngine,
    UserConfig::language.Streaming,         UserConfig::language.GetHelp,
    UserConfig::language.Restart,
};

#endif  // OSSM_SOFTWARE_MENU_H
