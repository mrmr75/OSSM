#ifndef OSSM_STATE_ACTIONS_H
#define OSSM_STATE_ACTIONS_H
#include "context.h"

// Forward declarations for action implementations (defined in actions.cpp)
void ossmDrawHello();
void ossmDrawMenu();
void ossmStartHoming();
void ossmStopHoming();
void ossmDrawPlayControls();
void ossmStartStreaming();
void ossmDrawPatternControls();
void ossmDrawPreflight();
void ossmResetSettingsStrokeEngine();
void ossmResetSettingsSimplePen();
void ossmResetSettingsStreaming();
void ossmIncrementControlStrokeEngine();
void ossmIncrementControlStreaming();
void ossmStartSimplePenetration();
void ossmStartStrokeEngine();
void ossmEmergencyStop();
void ossmDrawHelp();
void ossmDrawWiFi();
void ossmDrawUpdate();
void ossmDrawNoUpdate();
void ossmDrawUpdating();
void ossmDrawError();
void ossmSetHomed();
void ossmSetNotHomed();
void ossmCheckPairing();
void ossmDrawPairingSuccess();
void ossmResetWiFi();
void ossmRestart();
void ossmPauseStrokeEngine();
void ossmResumeStrokeEngine();
void ossmEndStrokeEngineAndReturnHome();
void ossmDrawPauseMenu();
void ossmDrawCalibrationControls();
void ossmEndCalibrationMode();
void ossmDrawCalibrationMenu();
void ossmEndCalibrationMenu();
void ossmStoreCalibrationPoint();
void ossmPauseSimplePenetration();
void ossmResumeSimplePenetration();
void ossmEndSimplePenetrationAndReturnHome();

namespace actions {

    auto setCaller = [](ReturnState c) {
        return [c](MachineContext& ctx) { ctx.returnState = c; };
    };

    constexpr auto drawHello = []() { ossmDrawHello(); };

    constexpr auto drawMenu = []() { ossmDrawMenu(); };

    constexpr auto startHoming = []() { ossmStartHoming(); };

    constexpr auto stopHoming = []() { ossmStopHoming(); };

    constexpr auto drawPlayControls = []() { ossmDrawPlayControls(); };

    constexpr auto startStreaming = []() { ossmStartStreaming(); };

    constexpr auto drawPatternControls = []() { ossmDrawPatternControls(); };

    constexpr auto drawPreflight = []() { ossmDrawPreflight(); };

    constexpr auto resetSettingsStrokeEngine = []() { ossmResetSettingsStrokeEngine(); };

    constexpr auto resetSettingsSimplePen = []() { ossmResetSettingsSimplePen(); };

    constexpr auto resetSettingsStreaming = []() { ossmResetSettingsStreaming(); };

    constexpr auto incrementControlStrokeEngine = []() { ossmIncrementControlStrokeEngine(); };

    constexpr auto incrementControlStreaming = []() { ossmIncrementControlStreaming(); };

    constexpr auto startSimplePenetration = []() { ossmStartSimplePenetration(); };

    constexpr auto startStrokeEngine = []() { ossmStartStrokeEngine(); };

    constexpr auto emergencyStop = []() { ossmEmergencyStop(); };

    constexpr auto drawHelp = []() { ossmDrawHelp(); };

    constexpr auto drawWiFi = []() { ossmDrawWiFi(); };

    constexpr auto drawUpdate = []() { ossmDrawUpdate(); };

    constexpr auto drawNoUpdate = []() { ossmDrawNoUpdate(); };

    constexpr auto drawUpdating = []() { ossmDrawUpdating(); };

    constexpr auto stopWifiPortal = []() {};

    constexpr auto resetWiFi = []() { ossmResetWiFi(); };

    constexpr auto drawError = []() { ossmDrawError(); };

    constexpr auto checkPairing = []() { ossmCheckPairing(); };

    constexpr auto drawPairingSuccess = []() { ossmDrawPairingSuccess(); };

    constexpr auto setHomed = []() { ossmSetHomed(); };

    constexpr auto setNotHomed = []() { ossmSetNotHomed(); };

    constexpr auto restart = []() { ossmRestart(); };

    constexpr auto endCalibrationMode = []() { ossmEndCalibrationMode(); };

    constexpr auto drawCalibrationMenu = []() { ossmDrawCalibrationMenu(); };

    constexpr auto drawCalibrationControls = []() { ossmDrawCalibrationControls(); };

    constexpr auto endCalibrationMenu = []() { ossmEndCalibrationMenu(); };

    constexpr auto storeCalibrationPoint = []() { ossmStoreCalibrationPoint(); };

    // This is a more complex action that involves network communication, so we define it as a lambda function
    constexpr auto pauseStrokeEngine = []() { ossmPauseStrokeEngine(); };
    constexpr auto resumeStrokeEngine = []() { ossmResumeStrokeEngine(); };
    constexpr auto endStrokeEngineAndReturnHome = []() { ossmEndStrokeEngineAndReturnHome(); };
    constexpr auto drawPauseMenu = []() { ossmDrawPauseMenu(); };
    constexpr auto pauseSimplePenetration = []() { ossmPauseSimplePenetration(); };
    constexpr auto resumeSimplePenetration = []() { ossmResumeSimplePenetration(); };
    constexpr auto endSimplePenetrationAndReturnHome = []() { ossmEndSimplePenetrationAndReturnHome(); };

}  // namespace actions

#endif  // OSSM_STATE_ACTIONS_H
