#include "calibration.h"
#include "ossm/state/calibration.h"
#include "ossm/Events.h"
#include "ossm/state/settings.h"
#include "ossm/state/state.h"
#include "constants/Config.h"
#include "services/display.h"
#include "services/encoder.h"
#include "services/tasks.h"
#include "constants/LogTags.h"
#include "services/stepper.h"
#include "esp_log.h"

inline float maxPositionInSteps() {
    return max(calibration.measuredStrokeSteps, Config::Driver::minStrokeLengthMm);
}

namespace calibrations {

    long percentageToSteps(float percentage) {
        float maxSteps = maxPositionInSteps();
        // Clamp input percentage first
        float clamped = constrain(percentage, 0.0f, 100.0f);

        return static_cast<long>((clamped * maxSteps) / 100.0f);
    }

    float stepsToPercentage(long steps) {
        float maxSteps = maxPositionInSteps();
        if (maxSteps <= 0) return 0.0f; // Avoid division by zero

        // Clamp steps to valid range
        long clampedSteps = constrain(steps, 0L, static_cast<long>(maxSteps));

        return (clampedSteps * 100.0f) / maxSteps;
    }

    float justOutPositionPct() {
        return calibration.refs.justOutPosition.has_value() ?
                stepsToPercentage(calibration.refs.justOutPosition.value()) :
                0.0f;
    };
    float justInPositionPct() {
        return calibration.refs.justInPosition.has_value() ?
                constrain(stepsToPercentage(calibration.refs.justInPosition.value()), justOutPositionPct(), 100.0f) :
                constrain(justOutPositionPct() + stepsToPercentage(30_mm), 0.0f, 100.0f);
    };
    float maxDepthPositionPct() {
        return calibration.refs.maxDepthPosition.has_value() ?
                constrain(stepsToPercentage(calibration.refs.maxDepthPosition.value()), justInPositionPct(), 100.0f) :
                constrain(justOutPositionPct() + stepsToPercentage(150_mm), 0.0f, 100.0f);
    };
    float referencePositionPct() {
        return calibration.refs.referencePosition.has_value() ?
                constrain(stepsToPercentage(calibration.refs.referencePosition.value()), 0.0f, 100.0f) :
                constrain((justInPositionPct() + maxDepthPositionPct())/2, 0.0f, 100.0f);
    };

    ReferencePositions refs;

    void drawCalibrationControlsTask(void *pvParameters) {
        ESP_LOGI("Calibration", "Starting calibration controls task");

        // Initialize encoder for position control
        encoder.setBoundaries(0, 100, false);
        encoder.setAcceleration(0); // No acceleration for precise control

        // Get current stepper position and convert to percentage
        int startPercentage = stepsToPercentage(stepper->getCurrentPosition());

        encoder.setEncoderValue(startPercentage);
        settings.depth = startPercentage;
        Stroker.setupDepth();

        bool isFirstDraw = true;

        while (ulTaskNotifyTake(pdTRUE, 0) == 0) {
            bool shouldRedraw = isFirstDraw || encoder.encoderChanged();

            if (!shouldRedraw) {
                vTaskDelay(50);
                continue;
            }

            isFirstDraw = false;
            long encoderValue = encoder.readEncoder();

            if (xSemaphoreTake(displayMutex, 100) == pdTRUE) {
                clearPage(true, false);
                String title("Manual Mode");
                setHeader(title);



                // Draw position bar
                int barX = 10;
                int barY = 30;
                int barWidth = 108;
                int barHeight = 8;

                // Draw background bar (0 to max)
                display.drawFrame(barX, barY, barWidth, barHeight);

                // Draw filled portion based on current position
                int filledWidth = (encoderValue * barWidth) / 100;
                if (filledWidth > 0) {
                    display.drawBox(barX + 1, barY + 1, filledWidth, barHeight - 2);
                }

                // Draw calibration point tick marks and labels
                display.setFont(Config::Font::small);

                // Helper lambda to draw a tick mark at a given percentage position
                auto drawTickMark = [&](float percentage, const char* label) {
                    // Exact same calculation as filledWidth uses
                    int tickX = barX + static_cast<int>((percentage * barWidth) / 100.0f);
                    // Allow drawing ticks right at the edges
                    if (tickX >= barX && tickX <= barX + barWidth) {
                        display.drawLine(tickX, barY - 4, tickX, barY);
                        display.drawUTF8(tickX - 3, barY - 6, label);
                    }
                };

                // Draw tick marks for each calibration point
                if (calibration.refs.justOutPosition.has_value()) {
                    float val = stepsToPercentage(calibration.refs.justOutPosition.value());
                    ESP_LOGD("Calibration", "> = %.1f%% at %d px", val, barX + static_cast<int>((val * barWidth) / 100.0f));
                    drawTickMark(val, ">");
                }
                if (calibration.refs.justInPosition.has_value()) {
                    float val = stepsToPercentage(calibration.refs.justInPosition.value());
                    ESP_LOGD("Calibration", "I = %.1f%% at %d px", val, barX + static_cast<int>((val * barWidth) / 100.0f));
                    drawTickMark(val, "O");
                }
                if (calibration.refs.maxDepthPosition.has_value()) {
                    float val = stepsToPercentage(calibration.refs.maxDepthPosition.value());
                    ESP_LOGD("Calibration", "< = %.1f%% at %d px", val, barX + static_cast<int>((val * barWidth) / 100.0f));
                    drawTickMark(val, "<");
                }
                if (calibration.refs.referencePosition.has_value()) {
                    float val = stepsToPercentage(calibration.refs.referencePosition.value());
                    ESP_LOGD("Calibration", "I = %.1f%% at %d px", val, barX + static_cast<int>((val * barWidth) / 100.0f));
                    drawTickMark(val, "I");
                }
                ESP_LOGD("Calibration", "Current encoder value = %d%%, filledWidth = %d px", encoderValue, filledWidth);

                // Draw position text
                display.setFont(Config::Font::base);
                long positionSteps = percentageToSteps(encoderValue);
                float positionMm = positionSteps / Config::Driver::stepsPerMM;
                String positionText = String(positionMm , 1) + "mm / " + String(maxPositionInSteps() / Config::Driver::stepsPerMM, 1) + "mm";
                display.drawUTF8(10, 50, positionText.c_str());

                // Draw instructions
                display.drawUTF8(10, 60, String("Rotate to move").c_str());

                refreshPage(true, false);
                xSemaphoreGive(displayMutex);
            }

            ///stepper->moveTo(-positionSteps);
            settings.depth = encoderValue;
            vTaskDelay(10);
        };

        ESP_LOGI("Manual", "Exit signal received, cleaning up");

        Stroker.stopMotion();
        encoder.setEncoderValue(encoder.readEncoder());

        // Reset encoder boundaries to default
        encoder.setBoundaries(0, 100, false);
        encoder.setAcceleration(10);

        ESP_LOGI("Manual", "Manual mode complete");
        Tasks::activeUiTaskH = NULL;

        vTaskDelete(nullptr);
    }

    void drawCalibrationControls() {
        Tasks::startUiTask(drawCalibrationControlsTask, "drawCalibrationControlsTask", nullptr);
    }

    void endCalibrationMode() {
        Tasks::cancelActiveUiTask();
    }
}
