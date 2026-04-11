#ifndef UI_HELLO_ANIMATION_H
#define UI_HELLO_ANIMATION_H

#include "DisplayTypes.h"

namespace ui {

// Precomputed hello animation frames.
// Each frame holds Y positions for the four letters O, S, S, M.
// Letters enter one at a time with a bouncing drop effect, staggered by 3
// frames. A height of 0 means the letter is not yet visible.
static constexpr HelloFrame HELLO_FRAMES[] = {
    {{ -6,   0,   0,   0}},
    {{  0,   0,   0,   0}},
    {{ 12,   0,   0,   0}},
    {{ 36,   0,   0,   0}},
    {{ 32,   0,   0,   0}},
    {{ 30,  12,   0,   0}},
    {{ 32,  36,   0,   0}},
    {{ 36,  32,   0,   0}},
    {{ 40,  30,  12,   0}},
    {{ 40,  32,  36,   0}},
    {{ 40,  36,  32,   0}},
    {{ 40,  40,  30,  12}},
    {{ 40,  40,  32,  36}},
    {{ 40,  40,  36,  32}},
    {{ 40,  40,  40,  30}},
    {{ 40,  40,  40,  32}},
    {{ 40,  40,  40,  36}},
    {{ 40,  40,  40,  40}},
};

static constexpr int HELLO_FRAME_COUNT =
    sizeof(HELLO_FRAMES) / sizeof(HELLO_FRAMES[0]);

}  // namespace ui

#endif  // UI_HELLO_ANIMATION_H
