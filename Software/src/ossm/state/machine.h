#ifndef OSSM_STATE_MACHINE_H
#define OSSM_STATE_MACHINE_H

#include "boost/sml.hpp"

#include "actions.h"
#include "guards.h"
#include "../Events.h"
#include "../../utils/update.h"
#include "homing.h"
#include "preflight.h"
#include "pause.h"

namespace sml = boost::sml;

// Suppress known Boost.SML anonymous namespace warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsubobject-linkage"

struct OSSMStateMachine {
    auto operator()() const {
        using namespace sml;
        using namespace actions;
        using namespace guards;

        return make_transition_table(
        // clang-format off
#ifdef AJ_DEVELOPMENT_HARDWARE
            *"idle"_s + initDone = "menu"_s,
#else
            *"idle"_s + initDone / drawHello = "firstRun"_s,
#endif
            "firstRun"_s / setCaller(ReturnState::FirstRun) = state<HomingStateMachine>,
            state<HomingStateMachine> + event<Return> [isFrom(ReturnState::FirstRun)] = "menu"_s,

            state<PreflightStateMachine> + event<ReturnToMenu> = "menu"_s,
            "menu"_s / (drawMenu) = "menu.idle"_s,
            "menu.idle"_s + buttonPress[(isOption(Menu::SimplePenetration))] = "simplePenetration"_s,
            "menu.idle"_s + buttonPress[(isOption(Menu::StrokeEngine))] = "strokeEngine"_s,
            "menu.idle"_s + buttonPress[(isOption(Menu::Streaming))] = "streaming"_s,
            "menu.idle"_s + buttonPress[(isOption(Menu::Pairing)) && isOnline] = "pairing"_s,
            "menu.idle"_s + buttonPress[(isOption(Menu::Pairing)) && !isOnline] = "pairing.wifi"_s,
            "menu.idle"_s + buttonPress[(isOption(Menu::UpdateOSSM))] = "update"_s,
            "menu.idle"_s + buttonPress[(isOption(Menu::WiFiSetup))] = "wifi"_s,
            "menu.idle"_s + buttonPress[isOption(Menu::Help)] = "help"_s,
            "menu.idle"_s + buttonPress[(isOption(Menu::Restart))] = "restart"_s,

            "simplePenetration"_s [isNotHomed] / setCaller(ReturnState::SimplePenetration) = state<HomingStateMachine>,
            "simplePenetration"_s [!isPreflightSafe] / setCaller(ReturnState::SimplePenetration) = state<PreflightStateMachine>,
            state<HomingStateMachine> + event<Return> [isFrom(ReturnState::SimplePenetration)] = "simplePenetration"_s,
            state<PreflightStateMachine> + event<Return> [isFrom(ReturnState::SimplePenetration)] = "simplePenetration"_s,
            "simplePenetration"_s / (resetSettingsSimplePen, drawPlayControls, startSimplePenetration) = "simplePenetration.idle"_s,
            "simplePenetration.idle"_s + longPress / (pauseSimplePenetration, setCaller(ReturnState::SimplePenetration)) = state<PauseStateMachine>,
            "simplePenetration.idle"_s + event<ReturnToMenu> / emergencyStop = "menu"_s,
            state<PauseStateMachine> + event<Resume>       [isFrom(ReturnState::SimplePenetration)] / resumeSimplePenetration = "simplePenetration.idle"_s,
            state<PauseStateMachine> + event<ReturnToMenu> [isFrom(ReturnState::SimplePenetration)] / endSimplePenetrationAndReturnHome = "menu"_s,

            "strokeEngine"_s [isNotHomed] / setCaller(ReturnState::Stroke) = state<HomingStateMachine>,
            "strokeEngine"_s [!isPreflightSafe] / setCaller(ReturnState::Stroke) = state<PreflightStateMachine>,
            state<HomingStateMachine> + event<Return> [isFrom(ReturnState::Stroke)] = "strokeEngine"_s,
            state<PreflightStateMachine> + event<Return> [isFrom(ReturnState::Stroke)] = "strokeEngine"_s,
            "strokeEngine"_s / (resetSettingsStrokeEngine, drawPlayControls, startStrokeEngine) = "strokeEngine.idle"_s,
            "strokeEngine.idle"_s + buttonPress / incrementControlStrokeEngine = "strokeEngine.idle"_s,
            "strokeEngine.idle"_s + doublePress / drawPatternControls = "strokeEngine.pattern"_s,
            "strokeEngine.idle"_s + longPress / (pauseStrokeEngine, setCaller(ReturnState::Stroke)) = state<PauseStateMachine>,
            "strokeEngine.idle"_s + event<ReturnToMenu> / emergencyStop = "menu"_s,
            state<PauseStateMachine> + event<Resume>       [isFrom(ReturnState::Stroke)] / resumeStrokeEngine = "strokeEngine.idle"_s,
            state<PauseStateMachine> + event<ReturnToMenu> [isFrom(ReturnState::Stroke)] / endStrokeEngineAndReturnHome = "menu"_s,

            "strokeEngine.pattern"_s + buttonPress / drawPlayControls = "strokeEngine.idle"_s,
            "strokeEngine.pattern"_s + doublePress / drawPlayControls = "strokeEngine.idle"_s,
            "strokeEngine.pattern"_s + longPress / (emergencyStop, setNotHomed) = "menu"_s,
            "strokeEngine.pattern"_s + event<ReturnToMenu> / emergencyStop = "menu"_s,
            "strokeEngine.preflight"_s + event<ReturnToMenu> = "menu"_s,

            "streaming"_s [isNotHomed] / setCaller(ReturnState::Streaming) = state<HomingStateMachine>,
            "streaming"_s [!isPreflightSafe] / setCaller(ReturnState::Streaming) = state<PreflightStateMachine>,
            state<HomingStateMachine> + event<Return> [isFrom(ReturnState::Streaming)] = "streaming"_s,
            state<PreflightStateMachine> + event<Return> [isFrom(ReturnState::Streaming)] = "streaming"_s,
            "streaming"_s / (resetSettingsStreaming, drawPlayControls, startStreaming) = "streaming.idle"_s,
            "streaming.idle"_s + longPress / (emergencyStop, setNotHomed) = "menu"_s,
            "streaming.idle"_s + event<ReturnToMenu> / emergencyStop = "menu"_s,
            "streaming.idle"_s + buttonPress / incrementControlStreaming = "streaming.idle"_s,

            "pairing"_s / checkPairing = "pairing.idle"_s,
            "pairing.idle"_s + pairingDone = "pairing.success"_s,
            "pairing.idle"_s + buttonPress = "menu"_s,
            "pairing.idle"_s + longPress = "menu"_s,
            "pairing.idle"_s + error = "menu"_s,

            "pairing.success"_s / drawPairingSuccess = "pairing.success.idle"_s,
            "pairing.success.idle"_s + buttonPress = "menu"_s,
            "pairing.success.idle"_s + longPress = "menu"_s,

            "pairing.wifi"_s / drawWiFi = "pairing.wifi.idle"_s,
            "pairing.wifi.idle"_s + pairingDone = "pairing"_s,
            "pairing.wifi.idle"_s + buttonPress = "menu"_s,
            "pairing.wifi.idle"_s + longPress = "menu"_s,

            "update"_s [isOnline] / drawUpdate = "update.checking"_s,
            "update"_s = "wifi"_s,
            "update.checking"_s [isUpdateAvailable] / (drawUpdating, updateOSSM) = "update.updating"_s,
            "update.checking"_s / drawNoUpdate = "update.idle"_s,
            "update.idle"_s + buttonPress = "menu"_s,
            "update.updating"_s  = X,

            "wifi"_s / drawWiFi = "wifi.idle"_s,
            "wifi.idle"_s + wifiDone / stopWifiPortal = "menu"_s,
            "wifi.idle"_s + buttonPress / stopWifiPortal = "menu"_s,
            "wifi.idle"_s + longPress / resetWiFi = "restart"_s,

            "help"_s / drawHelp = "help.idle"_s,
            "help.idle"_s + buttonPress = "menu"_s,

            state<HomingStateMachine> + error = "error"_s,
            "error"_s / drawError = "error.idle"_s,
            "error.idle"_s + buttonPress / drawHelp = "error.help"_s,
            "error.help"_s + buttonPress / restart = X,

            "restart"_s / restart = X);

        // clang-format on
    }
};

#pragma GCC diagnostic pop

#endif  // OSSM_STATE_MACHINE_H
