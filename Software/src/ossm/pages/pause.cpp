#include "pause.h"
#include "ossm/menu/menu.h"
#include "services/tasks.h"

namespace pause_menu {
static const char* pauseMenuStrings[] = {
    "Resume",
    "End Session"
};

PauseMenu pauseMenuOption = PauseMenu::RESUME;

static void pauseMenuTask(void *pvParameters) {
    menu::drawMenuImpl("PAUSED", pauseMenuStrings, pauseMenuOption);
}

void drawPauseMenu() {
    Tasks::startUiTask(pauseMenuTask, "pauseMenuTask", nullptr);
}
}
