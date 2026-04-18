#include "calibration_menu.h"
#include "ossm/state/settings.h"
#include "ossm/state/calibration.h"
#include "ossm/menu/menu.h"
#include "constants/Config.h"
#include "services/display.h"
#include "services/encoder.h"
#include "services/tasks.h"
#include "components/HeaderBar.h"
#include "esp_log.h"
#include "ui.h"
#include "calibration.h"

namespace calibrations {

CalibrationMenuState calibrationMenuState;

void storeCalibrationPoint(CalibrationPointType type) {
    if (type == CalibrationPointType::Cancel) {
        ESP_LOGD("Manual", "Calibration cancelled - no point stored");
        return;
    }

    // Get current position from settings.depth which holds the actual manual position in percentage
    float positionPct = static_cast<float>(settings.depth);
    long positionSteps = percentageToSteps(positionPct);

    // Store the calibration point in calibration state as absolute steps
    switch (type) {
        case CalibrationPointType::JustOut:
            calibration.refs.justOutPosition = positionSteps;
            ESP_LOGI("Calibration", "Stored JustOut at %ld steps (%.1f%%)", positionSteps, positionPct);
            break;
        case CalibrationPointType::JustIn:
            calibration.refs.justInPosition = positionSteps;
            ESP_LOGI("Calibration", "Stored JustIn at %ld steps (%.1f%%)", positionSteps, positionPct);
            break;
        case CalibrationPointType::MaxDepth:
            calibration.refs.maxDepthPosition = positionSteps;
            ESP_LOGI("Calibration", "Stored MaxDepth at %ld steps (%.1f%%)", positionSteps, positionPct);
            break;
        case CalibrationPointType::Reference:
            calibration.refs.referencePosition = positionSteps;
            ESP_LOGI("Calibration", "Stored Reference at %ld steps (%.1f%%)", positionSteps, positionPct);
            break;
        case CalibrationPointType::Cancel:
            // Already handled above
            break;
        default:
            ESP_LOGW("Calibration", "Unknown calibration type: %d", static_cast<int>(type));
            break;
    }
}

static void drawCalibrationMenuTask(void *pvParameters) {
    ESP_LOGI("Calibration", "Starting calibration menu task");

    // Use the existing menu template
    menu::drawMenuImpl("Store Calibration", calibrationPointStrings, calibrationMenuState.currentOption);
}

void drawCalibrationMenu() {
    Tasks::startUiTask(drawCalibrationMenuTask, "drawCalibrationMenuTask", nullptr);
}

void endCalibrationMenu() {
    Tasks::cancelActiveUiTask();
}

} // namespace calibrations
