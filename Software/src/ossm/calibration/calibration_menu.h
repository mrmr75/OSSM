#ifndef OSSM_MANUAL_CALIBRATION_MENU_H
#define OSSM_MANUAL_CALIBRATION_MENU_H

#include "calibration_types.h"

namespace calibrations {

/**
 * State for the calibration point selection menu
 */
struct CalibrationMenuState {
    CalibrationPointType currentOption = CalibrationPointType::JustOut;
};

extern CalibrationMenuState calibrationMenuState;

/**
 * Draw the calibration point selection menu
 * Uses the existing menu template for consistency
 */
void drawCalibrationMenu();

/**
 * End the calibration menu and return to manual mode
 */
void endCalibrationMenu();

/**
 * Store the calibration point at the current encoder position
 * @param type The type of calibration point to store
 */
void storeCalibrationPoint(CalibrationPointType type);

} // namespace manual

#endif // OSSM_MANUAL_CALIBRATION_MENU_H
