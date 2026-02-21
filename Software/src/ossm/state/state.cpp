#include "state.h"

#include "../Events.h"

StateLogger stateLogger;

volatile bool clickEventsEnabled = true; // Default to true, can be set to false during certain operations like homing

// Static pointer to hold the state machine instance
sml::sm<OSSMStateMachine, sml::thread_safe<ESP32RecursiveMutex>,
        sml::logger<StateLogger>> *stateMachine = nullptr;

void initStateMachine() {
    if (stateMachine == nullptr) {
        stateMachine = new sml::sm<OSSMStateMachine,
                                   sml::thread_safe<ESP32RecursiveMutex>,
                                   sml::logger<StateLogger>>(stateLogger);

        stateMachine->process_event(InitDone{});
    }
}
