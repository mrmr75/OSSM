#ifndef OSSM_STATE_CALIBRATION_H
#define OSSM_STATE_CALIBRATION_H

#include <Arduino.h>
#include "boost/sml.hpp"

#include "actions.h"
#include "guards.h"
#include "../Events.h"
#include "homing.h"
#include "preflight.h"
#include "../../structs/ReferencePositions.h"

/**
 * Calibration state - tracks homing and calibration data
 * This state persists across sessions until device is re-homed
 */
struct CalibrationState {
    float currentSensorOffset = 0;
    float measuredStrokeSteps = 0;
    bool isHomed = false;
    bool isForward = true;  // Homing direction
    ReferencePositions refs;
    float speedScale = 1.0; // Additional field for speed scaling
};

extern CalibrationState calibration;

// Suppress known Boost.SML anonymous namespace warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsubobject-linkage"

struct CalibrationStateMachine {
    auto operator()() const {
        using namespace sml;
        using namespace actions;
        using namespace guards;

        return make_transition_table(
            *"calibration"_s = "calibration.check"_s,

            "calibration.check"_s [isNotHomed] / setCaller(ReturnState::Calibration) = state<HomingStateMachine>,
            "calibration.check"_s [!isPreflightSafe] / setCaller(ReturnState::Calibration) = state<PreflightStateMachine>,
            state<HomingStateMachine> + event<Return> [isFrom(ReturnState::Calibration)] = "calibration.check"_s,
            state<PreflightStateMachine> + event<Return> [isFrom(ReturnState::Calibration)] = "calibration.check"_s,

            "calibration.check"_s / (resetSettingsStrokeEngine, startStrokeEngine) = "calibration.starting"_s,
            "calibration.starting"_s + strokeEngineReady / drawCalibrationControls = "calibration.idle"_s,
            "calibration.idle"_s + buttonPress / (endCalibrationMode, drawCalibrationMenu) = "calibration.calibration_menu"_s,
            "calibration.idle"_s + doublePress / (endCalibrationMode, drawPlayControls, sml::process(CalibrationDone{})) = X,
            "calibration.idle"_s + longPress / (emergencyStop, restart, sml::process(ReturnToMenu{})) = X,
            "calibration.calibration_menu"_s + buttonPress / (storeCalibrationPoint, endCalibrationMenu, drawCalibrationControls) = "calibration.idle"_s,
            "calibration.calibration_menu"_s + longPress / (endCalibrationMenu, drawCalibrationControls) = "calibration.idle"_s
        );
    }
};

#pragma GCC diagnostic pop

#endif  // OSSM_STATE_CALIBRATION_H
