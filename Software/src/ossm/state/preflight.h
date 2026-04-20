#ifndef OSSM_STATE_PREFLIGHT_H
#define OSSM_STATE_PREFLIGHT_H

#include "boost/sml.hpp"
#include "../Events.h"
#include "actions.h"
#include "guards.h"

// Suppress known Boost.SML anonymous namespace warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsubobject-linkage"

struct PreflightStateMachine {
    auto operator()() const {
        using namespace sml;
        using namespace actions;
        using namespace guards;

        return make_transition_table(
            *"preflight"_s / drawPreflight = "preflight.idle"_s,
            "preflight.idle"_s + longPress / sml::process(ReturnToMenu{}) = X,
            "preflight.idle"_s + preflightDone / (sml::process(Return{})) = X
        );
    }
};

#pragma GCC diagnostic pop

#endif  // OSSM_STATE_PREFLIGHT_H
