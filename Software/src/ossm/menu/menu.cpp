#include "menu.h"

#include <WiFi.h>

#include "constants/Menu.h"
#include "ossm/state/menu.h"
#include "ossm/state/state.h"
#include "services/display.h"
#include "services/encoder.h"
#include "services/tasks.h"
#include "ui.h"
#include "utils/analog.h"
#include "components/HeaderBar.h"
#include "ossm/calibration/calibration_types.h"
#include "ossm/pages/pause.h"

namespace sml = boost::sml;
using namespace sml;

namespace menu {

template<typename MenuEnum, size_t N>
void drawMenuImpl(const char* headerText,
                        const char* const (&menuStrings)[N],
                        MenuEnum& currentOption) {
    bool isFirstDraw = true;

    int clicksPerRow = 3;
    const int maxClicks = clicksPerRow * N;
    // Last Wifi State
    wl_status_t wifiState = WL_IDLE_STATUS;

    encoder.setBoundaries(0, maxClicks - 1, true);
    encoder.setEncoderValue((int)currentOption * clicksPerRow);
    encoder.setAcceleration(0);

    int lastEncoderValue = encoder.readEncoder();
    int currentEncoderValue;

    showHeaderIcons = true;

    while (ulTaskNotifyTake(pdTRUE, 0) == 0) {
        wl_status_t newWifiState = WiFiClass::status();

        bool shouldRedraw = isFirstDraw || encoder.encoderChanged() ||
                            (wifiState != newWifiState);

        if (!shouldRedraw) {
            vTaskDelay(50);
            continue;
        }

        wifiState = newWifiState;
        isFirstDraw = false;
        currentEncoderValue = encoder.readEncoder();

        if (xSemaphoreTake(displayMutex, 100) == pdTRUE) {


            if (abs(currentEncoderValue % maxClicks -
                    lastEncoderValue % maxClicks) >= clicksPerRow) {
                lastEncoderValue = currentEncoderValue % maxClicks;
                currentOption = (MenuEnum)floor(lastEncoderValue / clicksPerRow);
            }

            ESP_LOGD("Menu",
                     "currentEncoderValue: %d, lastEncoderValue: %d, "
                     "menuOption: %d",
                     currentEncoderValue, lastEncoderValue, currentOption);

            ui::MenuData data{};
            data.headerText = headerText;
            data.items = menuStrings;
            data.numItems = N;
            data.selectedIndex = (int)currentOption;

            ui::drawMenu(display.getU8g2(), data);
            refreshPage(true, true);
            xSemaphoreGive(displayMutex);
        }

        vTaskDelay(1);
    };
    Tasks::activeUiTaskH = NULL;
    vTaskDelete(nullptr);
}

static void drawMenuTask(void *pvParameters) {
     drawMenuImpl("Main Menu", menuStrings, menuState.currentOption);
}

void drawMenu() {
    Tasks::startUiTask(drawMenuTask, "drawMenuTask", nullptr);
}

template void drawMenuImpl<Menu, 8u>(const char* headerText,
                                     const char* const (&menuStrings)[8u],
                                     Menu& currentOption);

template void menu::drawMenuImpl<pause_menu::PauseMenu, 2u>(char const*, char const* const (&) [2u], pause_menu::PauseMenu&);
// Explicit template instantiation for CalibrationPointType
template void menu::drawMenuImpl<calibrations::CalibrationPointType, calibrations::NUM_TYPES>(char const*, char const* const (&) [calibrations::NUM_TYPES], calibrations::CalibrationPointType&);

}  // namespace menu
