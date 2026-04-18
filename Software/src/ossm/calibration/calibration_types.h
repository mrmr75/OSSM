#ifndef OSSM_MANUAL_CALIBRATION_TYPES_H
#define OSSM_MANUAL_CALIBRATION_TYPES_H

#include <Arduino.h>

namespace calibrations {

/**
 * Calibration point types for manual calibration mode
 * These represent different reference positions that can be stored
 */
enum CalibrationPointType {
    JustOut,    // Just out position
    JustIn,     // Just in position
    MaxDepth,   // Maximum depth position
    Reference,  // Reference position
    Cancel,     // Cancel without storing
    NUM_TYPES
};

// String representations for display (stored in PROGMEM)
static const char calPointJustOut[] PROGMEM = "Just Out";
static const char calPointJustIn[] PROGMEM = "Just In";
static const char calPointMaxDepth[] PROGMEM = "Max Depth";
static const char calPointReference[] PROGMEM = "Reference";
static const char calPointCancel[] PROGMEM = "Cancel";

// Array of calibration point strings for menu display
static const char* calibrationPointStrings[CalibrationPointType::NUM_TYPES] = {
    calPointJustOut,
    calPointJustIn,
    calPointMaxDepth,
    calPointReference,
    calPointCancel
};

} // namespace manual

#endif // OSSM_MANUAL_CALIBRATION_TYPES_H
