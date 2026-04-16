#ifndef OSSM_STATE_PAUSE_H
#define OSSM_STATE_PAUSE_H

#include "boost/sml.hpp"
#include "../Events.h"
#include "actions.h"
#include "guards.h"
#include "../pages/pause.h"

// Suppress known Boost.SML anonymous namespace warning
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsubobject-linkage"

struct PauseStateMachine {
    auto operator()() const {
        using namespace sml;
        using namespace actions;
        using namespace guards;

        return make_transition_table(
            *"pause"_s / drawPauseMenu = "pause.idle"_s,
            "pause.idle"_s + buttonPress[isPauseOption(pause_menu::PauseMenu::RESUME)]       / sml::process(Resume{})        = X,
            "pause.idle"_s + longPress                                                        / sml::process(Resume{})        = X,
            "pause.idle"_s + buttonPress[isPauseOption(pause_menu::PauseMenu::END_SESSION)]  / sml::process(ReturnToMenu{})  = X
        );
    }
};

#pragma GCC diagnostic pop

#endif  // OSSM_STATE_PAUSE_H
