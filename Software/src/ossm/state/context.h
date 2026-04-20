#ifndef OSSM_STATE_CONTEXT_H
#define OSSM_STATE_CONTEXT_H

enum class ReturnState { FirstRun, Stroke, SimplePenetration, Calibration, Streaming };

struct MachineContext {
    ReturnState returnState = ReturnState::FirstRun;
};

#endif // OSSM_STATE_CONTEXT_H
