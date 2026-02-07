#ifndef OSSM_SOFTWARE_CALIBRATION_MENU_H
#define OSSM_SOFTWARE_CALIBRATION_MENU_H

#include <Arduino.h>

#include "constants/UserConfig.h"

enum CalibrationMenu {
    CalibrateLength,
    SaveCalibration,
    UpdateOSSM,
    WiFiSetup,
    NUM_CALIBRATION_OPTIONS
};

static const char* calibrationMenuStrings[CalibrationMenu::NUM_CALIBRATION_OPTIONS] = {
    UserConfig::language.CalibrateLength,
    UserConfig::language.SaveCalibration,
    UserConfig::language.Update,
    UserConfig::language.WiFiSetup,
};

#endif  // OSSM_SOFTWARE_CALIBRATION_MENU_H
