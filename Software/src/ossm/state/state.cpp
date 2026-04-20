#include "state.h"
#include "../Events.h"
#include "context.h"

StateLogger stateLogger;
MachineContext context;

// Static pointer to hold the state machine instance
sml::sm<
    OSSMStateMachine,
    sml::thread_safe<ESP32RecursiveMutex>,
    sml::logger<StateLogger>,
    sml::process_queue<std::queue>
    > *stateMachine = nullptr;

void initStateMachine() {
    if (stateMachine == nullptr) {

        stateMachine = new sml::sm<OSSMStateMachine,
                                   sml::thread_safe<ESP32RecursiveMutex>,
                                   sml::logger<StateLogger>,
                                   sml::process_queue<std::queue>
                                   >{stateLogger, context};

        stateMachine->process_event(InitDone{});
    }
}
