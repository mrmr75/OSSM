#ifndef OSSM_STATE_HOMING_H
#define OSSM_STATE_HOMING_H

#include <stdint.h>
#include "actions.h"
#include "guards.h"
#include "../Events.h"
#include "../homing/homing.h"

// Suppress known Boost.SML anonymous namespace warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsubobject-linkage"

struct HomingStateMachine {
    auto operator()() const {
        using namespace sml;
        using namespace actions;
        using namespace guards;

        return make_transition_table(
            *"homing"_s / (startHoming, []{ homing::homingDirection = -1; }) = "homing.forward"_s,
            "homing.forward"_s + error = "error"_s,
            "homing.forward"_s + buttonPress / stopHoming = "homing.forward"_s,
            "homing.forward"_s + homingDone / (startHoming, []{ homing::homingDirection = 1; }) = "homing.backward"_s,
            "homing.backward"_s + error = "error"_s,
            "homing.backward"_s + buttonPress / stopHoming = "homing.backward"_s,
            "homing.backward"_s + homingDone[(isStrokeTooShort)] = "error"_s,
            "homing.backward"_s + homingDone / (setHomed, sml::process(Return{})) = X,
            "error"_s / sml::process(Error{}) = X
        );
    }
};

#pragma GCC diagnostic pop

#endif // OSSM_STATE_HOMING_H
