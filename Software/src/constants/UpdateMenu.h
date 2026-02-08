#ifndef OSSM_SOFTWARE_UPDATE_MENU_H
#define OSSM_SOFTWARE_UPDATE_MENU_H

#include <Arduino.h>

#include "constants/UserConfig.h"

enum UpdateMenu {
    HTTP_Update,
    OTA_Update,
    BackToCalibration,
    NUM_UPDATE_OPTIONS
};

static const char* updateMenuStrings[UpdateMenu::NUM_UPDATE_OPTIONS] = {
    UserConfig::language.HTTP_Update,
    UserConfig::language.OTA_Update,
    UserConfig::language.Back,
};

#endif  // OSSM_SOFTWARE_UPDATE_MENU_H
