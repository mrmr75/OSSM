#include "simple_penetration.h"

#include "simple_pen_logic.h"
#include "constants/Config.h"
#include "ossm/state/calibration.h"
#include "ossm/state/session.h"
#include "ossm/state/settings.h"
#include "ossm/state/state.h"
#include "services/communication/nimble.h"
#include "services/communication/queue.h"
#include "services/stepper.h"
#include "services/tasks.h"

namespace sml = boost::sml;
using namespace sml;

namespace simple_penetration {

static volatile bool isPaused = false;

static void startSimplePenetrationTask(void *pvParameters) {
    int fullStrokeCount = 0;
    static int32_t targetPosition = 0;

    double lastSpeed = 0;

    bool stopped = false;

    while (ulTaskNotifyTake(pdTRUE, 0) == 0) {
        auto speed = simple_pen_logic::calculateSpeed(
            settings.speed, Config::Driver::maxSpeedMmPerSecond, (1_mm));
        auto acceleration = simple_pen_logic::calculateAcceleration(
            settings.speed, Config::Driver::maxSpeedMmPerSecond,
            Config::Advanced::accelerationScaling, (1_mm));

        bool isSpeedZero = simple_pen_logic::isInDeadZone(
            settings.speedKnob, Config::Advanced::commandDeadZonePercentage);
        bool isSpeedChanged =
            !isSpeedZero && simple_pen_logic::isSpeedChangeSignificant(
                                lastSpeed, speed,
                                Config::Advanced::commandDeadZonePercentage);
        bool isAtTarget =
            abs(targetPosition - stepper->getCurrentPosition()) == 0;

        // If the speed is zero, then stop the stepper and wait for the next
        if (isSpeedZero || isPaused) {
            stepper->stopMove();
            stopped = true;
            vTaskDelay(100);
            continue;
        } else if (stopped) {
            stepper->moveTo(targetPosition, false);
            stopped = false;
        }

        // If the speed is greater than the dead-zone, and the speed has changed
        // by more than the dead-zone, then update the stepper.
        // This must be done in the same task that the stepper is running in.
        if (isSpeedChanged) {
            lastSpeed = speed;
            stepper->setAcceleration(acceleration);
            stepper->setSpeedInHz(speed);
        }

        // If the stepper is not at the target, then wait for the next loop
        if (!isAtTarget) {
            vTaskDelay(1);
            // more than zero
            continue;
        }

        bool nextDirection = !calibration.isForward;
        calibration.isForward = nextDirection;

        targetPosition = simple_pen_logic::calculateTarget(
            calibration.isForward, settings.stroke,
            calibration.measuredStrokeSteps);

        ESP_LOGV("SimplePenetration", "target: %f,\tspeed: %f,\tacc: %f",
                 targetPosition, speed, acceleration);

        stepper->moveTo(targetPosition, false);

        if (settings.speed > Config::Advanced::commandDeadZonePercentage &&
            settings.stroke >
                (long)Config::Advanced::commandDeadZonePercentage) {
            fullStrokeCount++;
            session.strokeCount = floor(fullStrokeCount / 2);

            // This calculation assumes that at the end of every stroke you have
            // a whole positive distance, equal to maximum target position.
            session.distanceMeters +=
                (((float)settings.stroke / 100.0) *
                 calibration.measuredStrokeSteps / (1_mm)) /
                1000.0;
        }

        vTaskDelay(1);
    }
    Tasks::activeBackgroundTaskH = nullptr;
    vTaskDelete(nullptr);
}

void startSimplePenetration() {
    isPaused = false;
    int stackSize = 10 * configMINIMAL_STACK_SIZE;

    Tasks::startBackgroundTask(startSimplePenetrationTask,
                            "startSimplePenetrationTask", nullptr);
}

void pauseSimplePenetration() {
    isPaused = true;
    stepper->stopMove();
    ESP_LOGI("SimplePenetration", "Simple penetration paused");
}

void resumeSimplePenetration() {
    isPaused = false;
    // The background task will naturally resume movement on the next loop
    // iteration once the stepper finishes stopping.
    ESP_LOGI("SimplePenetration", "Simple penetration resumed");
}

void returnHome() {
    stepper->stopMove();
    stepper->setSpeedInHz(25.0 * (1_mm));
    stepper->moveTo(0, false);
    ESP_LOGI("SimplePenetration", "Simple penetration returning home");
}

}  // namespace simple_penetration
