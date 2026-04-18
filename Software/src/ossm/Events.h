#ifndef OSSM_SOFTWARE_EVENTS_H
#define OSSM_SOFTWARE_EVENTS_H

#include "boost/sml.hpp"
namespace sml = boost::sml;

/**
 * These are the events that the OSSM state machine can respond to.
 * They are used in OSSM.h and can be called from anywhere in the code that has
 * access to an OSSM state machine
 *
 * For Example:
 *  ossm->sm.process_event(ButtonPress{});
 *
 *
 * There's nothing special about these events, they are just structs.
 * They just happen to be defined inside of the OSSM State Machine class.
 */
struct ButtonPress {};
struct LongPress {};

struct DoublePress {};

// Distinct Done events for different phases (better type safety)
struct InitDone {};        // Initial transition from idle
struct HomingDone {};      // Homing forward/backward complete
struct PreflightDone {};  // Preflight check complete
struct WifiDone {};       // WiFi portal done
struct PairingDone {};       // BLE pairing done

struct Error {};

struct BleClick {};
struct ReturnToMenu {};

struct CalibrationDone {};
struct StrokeEngineReady{};
struct Return{};
struct Resume{};

// Definitions to make the table easier to read.
static auto bleClick = sml::event<BleClick>;
static auto buttonPress = sml::event<ButtonPress>;
static auto longPress = sml::event<LongPress>;
static auto doublePress = sml::event<DoublePress>;
static auto initDone = sml::event<InitDone>;
static auto homingDone = sml::event<HomingDone>;
static auto preflightDone = sml::event<PreflightDone>;
static auto pairingDone = sml::event<PairingDone>;
static auto wifiDone = sml::event<WifiDone>;
static auto error = sml::event<Error>;
static auto returnToMenu = sml::event<ReturnToMenu>;
static auto strokeEngineReady = sml::event<StrokeEngineReady>;
static auto calibrationDone = sml::event<CalibrationDone>;
static auto resume = sml::event<Resume>;
#endif  // OSSM_SOFTWARE_EVENTS_H
