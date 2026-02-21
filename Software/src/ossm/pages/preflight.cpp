#include "preflight.h"

#include "Strings.h"
#include "constants/Config.h"
#include "constants/Menu.h"
#include "constants/Pins.h"
#include "ossm/Events.h"
#include "ossm/state/menu.h"
#include "ossm/state/state.h"
#include "services/display.h"
#include "services/stepper.h"
#include "services/tasks.h"
#include "ui.h"
#include "utils/analog.h"
#include "utils/format.h"
#include "components/HeaderBar.h"

namespace sml = boost::sml;
using namespace sml;

namespace pages {

static void drawPreflightTask(void *pvParameters) {
    auto menuString = menuStrings[menuState.currentOption];
    float speedPercentage;


    showHeaderIcons = true;

    do {
#ifdef AJ_DEVELOPMENT_HARDWARE
        speedPercentage = 0;
#else
        speedPercentage =
            getAnalogAveragePercent(SampleOnPin{Pins::Remote::speedPotPin, 50});
#endif
        if (speedPercentage < Config::Advanced::commandDeadZonePercentage) {
            Tasks::activeUiTaskH = NULL;
            stateMachine->process_event(Done{});
            vTaskDelete(nullptr);
            return;
        };

        if (xSemaphoreTake(displayMutex, 100) == pdTRUE) {
            ui::PreflightData data{menuString, speedPercentage,
                                   ui::strings::speed,
                                   ui::strings::speedWarning};
            ui::drawPreflight(display.getU8g2(), data);
            refreshPage(true, true);
            xSemaphoreGive(displayMutex);
        }

        vTaskDelay(100);
    } while (ulTaskNotifyTake(pdTRUE, 0) == 0);
    Tasks::activeUiTaskH = NULL;
    vTaskDelete(nullptr);
};

void drawPreflight() {
    Tasks::startUiTask(drawPreflightTask, "drawPreflightTask", nullptr);
}

}  // namespace pages
