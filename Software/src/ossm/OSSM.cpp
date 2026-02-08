#include "OSSM.h"

#include "constants/Images.h"
#include "constants/UserConfig.h"
#include "extensions/u8g2Extensions.h"
#include "services/encoder.h"

// Include NVS headers after all other includes to avoid conflicts
#include <nvs.h>
#include <nvs_flash.h>

namespace sml = boost::sml;
using namespace sml;

OSSM *ossm = nullptr;// NVS constant definitions
const char* OSSM::NVS_NAMESPACE = "ossm";
const char* OSSM::NVS_KEY_CALIBRATION_DATA = "calibration_data";
// Static member definitions
SettingPercents OSSM::setting = {.speed = 0,
                                 .stroke = 50,
                                 .sensation = 50,
                                 .depth = 10,
                                 .pattern = StrokePatterns::SimpleStroke};
bool OSSM::isCalibrated = false;

// Now we can define the OSSM constructor since OSSMStateMachine::operator() is
// fully defined
OSSM::OSSM(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &display,
           AiEsp32RotaryEncoder &encoder, FastAccelStepper *stepper)
    : display(display),
      encoder(encoder),
      stepper(stepper),
      sm(std::make_unique<
          sml::sm<OSSMStateMachine, sml::thread_safe<ESP32RecursiveMutex>,
                  sml::logger<StateLogger>>>(logger, *this)) {

    // Initialize NVS and load saved calibration data
    if (initNVS()) {
        loadCalibrationData();
        ESP_LOGD("OSSM", "NVS initialization and data loading completed");
    } else {
        ESP_LOGE("OSSM", "NVS initialization failed, using default values");
    }

    // All initializations are done, so start the state machine.
    sm->process_event(InitDone{});
}

/**
 * This task will write the word "OSSM" to the screen
 * then briefly show the RD logo.
 * and then end on the Kinky Makers logo.
 *
 * The Kinky Makers logo will stay on the screen until the next state is ready
 * :).
 * @param pvParameters
 */
void OSSM::drawHelloTask(void *pvParameters) {
    // parse ossm from the parameters
    OSSM *ossm = (OSSM *)pvParameters;

    int frameIdx = 0;
    const int nFrames = 8;

    int startX = 24;
    int offsetY = 12;

    // Bounce the Y position from 0 to 32, up to 24 and down to 32
    std::array<int, 8> framesY = {6, 12, 24, 48, 44, 42, 44, 48};
    std::array<int, 4> heights = {0, 0, 0, 0};
    int letterSpacing = 20;

    while (frameIdx < nFrames + 9) {
        if (frameIdx < nFrames) {
            heights[0] = framesY[frameIdx] - offsetY;
        }
        if (frameIdx - 3 > 0 && frameIdx - 3 < nFrames) {
            heights[1] = framesY[frameIdx - 3] - offsetY;
        }
        if (frameIdx - 6 > 0 && frameIdx - 6 < nFrames) {
            heights[2] = framesY[frameIdx - 6] - offsetY;
        }
        if (frameIdx - 9 > 0 && frameIdx - 9 < nFrames) {
            heights[3] = framesY[frameIdx - 9] - offsetY;
        }
        // increment the frame index
        frameIdx++;

        if (xSemaphoreTake(displayMutex, 100) == pdTRUE) {
            clearPage(true, true);
            ossm->display.setFont(u8g2_font_maniac_tf);
            ossm->display.drawUTF8(startX, heights[0], "O");
            ossm->display.drawUTF8(startX + letterSpacing, heights[1], "S");
            ossm->display.drawUTF8(startX + letterSpacing * 2, heights[2], "S");
            ossm->display.drawUTF8(startX + letterSpacing * 3, heights[3], "M");
            refreshPage(true, true);
            xSemaphoreGive(displayMutex);
        }
        // Saying hi to the watchdog :).
        vTaskDelay(1);
    };

    // Delay for a second, then show the RDLogo.
    vTaskDelay(1500);

    if (xSemaphoreTake(displayMutex, 100) == pdTRUE) {
        clearPage(true, true);
        drawStr::title("Research & Desire         ");   // Padding to offset from BLE icons
        ossm->display.drawXBMP(35, 14, 57, 50, Images::RDLogo);
        refreshPage(true, true);
        xSemaphoreGive(displayMutex);
    }

    vTaskDelay(1000);

    if (xSemaphoreTake(displayMutex, 100) == pdTRUE) {
        clearPage(true, true);
        drawStr::title("Kinky Makers       ");   // Padding to offset from BLE icons
        ossm->display.drawXBMP(40, 14, 50, 50, Images::KMLogo);
        refreshPage(true, true);
        xSemaphoreGive(displayMutex);
    }

    vTaskDelay(1000);

    if (xSemaphoreTake(displayMutex, 100) == pdTRUE) {
        clearPage(true, true);
        std::string measuringStrokeTitle = std::string(UserConfig::language.MeasuringStroke) + "         ";   // Padding to offset from BLE icons
        drawStr::title(measuringStrokeTitle.c_str());
        ossm->display.drawXBMP(40, 14, 50, 50, Images::KMLogo);
        refreshPage(true, true);
        xSemaphoreGive(displayMutex);
    }

    // Signal that drawHello is complete
    ossm->sm->process_event(DrawHelloDone{});

    // delete the task
    vTaskDelete(nullptr);
}

void OSSM::drawHello() {
    // 3 x minimum stack
    int stackSize = 3 * configMINIMAL_STACK_SIZE;
    xTaskCreate(drawHelloTask, "drawHello", stackSize, this, 1,
                &Tasks::drawHelloTaskH);
}

void OSSM::drawError() {
    // Throw the e-break on the stepper
    try {
        stepper->forceStop();
    } catch (const std::exception &e) {
        ESP_LOGD("OSSM::drawError", "Caught exception: %s", e.what());
    }

    if (xSemaphoreTake(displayMutex, 100) == pdTRUE) {
        clearPage(true, true);
        drawStr::title(UserConfig::language.Error);
        drawStr::multiLine(0, 20, errorMessage);
        refreshPage(true, true);
        xSemaphoreGive(displayMutex);
    }
}

void OSSM::drawCalibrationMenuTask(void *pvParameters) {
    bool isFirstDraw = true;
    OSSM *ossm = (OSSM *)pvParameters;

    int lastEncoderValue = ossm->encoder.readEncoder();
    int currentEncoderValue;
    int clicksPerRow = 3;
    const int maxClicks = clicksPerRow * CalibrationMenu::NUM_CALIBRATION_OPTIONS - 1; // All calibration menu items
    // Last Wifi STate
    wl_status_t wifiState = WL_IDLE_STATUS;

    ossm->encoder.setBoundaries(0, maxClicks, true);
    ossm->encoder.setAcceleration(0);

    ossm->calibrationMenuOption = (CalibrationMenu)floor(ossm->encoder.readEncoder() / clicksPerRow);

    ossm->encoder.setAcceleration(0);

    auto isInCorrectState = [ossm]() {
        return ossm->isInMode(OSSMMode::CALIBRATION);
    };

    while (isInCorrectState()) {
        wl_status_t newWifiState = WiFiClass::status();

        // Force redraw on first draw or when conditions change
        bool shouldRedraw = isFirstDraw || ossm->encoder.encoderChanged() || (wifiState != newWifiState);

        if (!shouldRedraw) {
            vTaskDelay(50);
            continue;
        }

        wifiState = newWifiState;

        isFirstDraw = false;
        currentEncoderValue = ossm->encoder.readEncoder();

        if (xSemaphoreTake(displayMutex, 100) == pdTRUE) {
            clearPage(true, false); // Clear page content but preserve header icons

            // Drawing Variables.
            int leftPadding = 6;  // Padding on the left side of the screen
            int fontSize = 8;
            int itemHeight = 20;   // Height of each item
            int visibleItems = 3;  // Number of items visible on the screen

            auto calibrationMenuOption = ossm->calibrationMenuOption;
            if (abs(currentEncoderValue % maxClicks -
                    lastEncoderValue % maxClicks) >= clicksPerRow) {
                lastEncoderValue = currentEncoderValue % maxClicks;
                calibrationMenuOption = (CalibrationMenu)floor(lastEncoderValue / clicksPerRow);

                ossm->calibrationMenuOption = calibrationMenuOption;
            }

            ESP_LOGD(
                "Calibration Menu",
                "currentEncoderValue: %d, lastEncoderValue: %d, calibrationMenuOption: %d",
                currentEncoderValue, lastEncoderValue, calibrationMenuOption);

            drawShape::scroll(100 * ossm->encoder.readEncoder() /
                              (clicksPerRow * CalibrationMenu::NUM_CALIBRATION_OPTIONS - 1));
            const char *menuName = calibrationMenuStrings[calibrationMenuOption];
            ESP_LOGD("Calibration Menu", "Hovering over state: %s", menuName);

            // Loop around to make an infinite menu.
            int lastIdx =
                calibrationMenuOption - 1 < CalibrationMenu::CalibrateLength ? CalibrationMenu::SaveCalibration : calibrationMenuOption - 1;
            int nextIdx =
                calibrationMenuOption + 1 > CalibrationMenu::SaveCalibration ? CalibrationMenu::CalibrateLength : calibrationMenuOption + 1;

            ossm->display.setFont(Config::Font::base);

            // Draw the previous item
            if (lastIdx >= CalibrationMenu::CalibrateLength) {
                ossm->display.drawUTF8(leftPadding, itemHeight * (1),
                                       calibrationMenuStrings[lastIdx]);
            }

            // Draw the next item
            if (nextIdx <= CalibrationMenu::SaveCalibration) {
                ossm->display.drawUTF8(leftPadding, itemHeight * (3),
                                       calibrationMenuStrings[nextIdx]);
            }

            // Draw the current item
            ossm->display.setFont(Config::Font::bold);
            ossm->display.drawUTF8(leftPadding, itemHeight * (2), menuName);

            // Draw a rounded rectangle around the center item
            ossm->display.drawRFrame(
                0,
                itemHeight * (visibleItems / 2) - (fontSize - itemHeight) / 2,
                120, itemHeight, 2);

            // Draw Shadow.
            ossm->display.drawLine(2, 2 + fontSize / 2 + 2 * itemHeight, 119,
                                   2 + fontSize / 2 + 2 * itemHeight);
            ossm->display.drawLine(120, 4 + fontSize / 2 + itemHeight, 120,
                                   1 + fontSize / 2 + 2 * itemHeight);

            refreshPage(true, true); // Include both footer and header in refresh
            xSemaphoreGive(displayMutex);
        }

        vTaskDelay(1);
    };

    // Clear header icons when exiting menu
    if (xSemaphoreTake(displayMutex, 100) == pdTRUE) {
        clearIcons(); // Clear the header icons
        ossm->display.setMaxClipWindow(); // Reset clipping
        ossm->display.sendBuffer(); // Send the cleared buffer to display
        xSemaphoreGive(displayMutex);
    }

    vTaskDelete(nullptr);
}

void OSSM::drawCalibrationMenu() {
    int stackSize = 5 * configMINIMAL_STACK_SIZE;
    xTaskCreate(drawCalibrationMenuTask, "drawCalibrationMenuTask", stackSize, this, 1,
                &Tasks::drawCalibrationMenuTaskH);
}

void OSSM::drawUpdateMenuTask(void *pvParameters) {
    bool isFirstDraw = true;
    OSSM *ossm = (OSSM *)pvParameters;

    int lastEncoderValue = ossm->encoder.readEncoder();
    int currentEncoderValue;
    int clicksPerRow = 3;
    const int maxClicks = clicksPerRow * UpdateMenu::NUM_UPDATE_OPTIONS - 1; // All update menu items
    // Last Wifi STate
    wl_status_t wifiState = WL_IDLE_STATUS;

    ossm->encoder.setBoundaries(0, maxClicks, true);
    ossm->encoder.setAcceleration(0);

    ossm->updateMenuOption = (UpdateMenu)floor(ossm->encoder.readEncoder() / clicksPerRow);

    ossm->encoder.setAcceleration(0);

    auto isInCorrectState = [ossm]() {
        return ossm->isInMode(OSSMMode::UPDATE_MENU);
    };

    while (isInCorrectState()) {
        wl_status_t newWifiState = WiFiClass::status();

        // Force redraw on first draw or when conditions change
        bool shouldRedraw = isFirstDraw || ossm->encoder.encoderChanged() || (wifiState != newWifiState);

        if (!shouldRedraw) {
            vTaskDelay(50);
            continue;
        }

        wifiState = newWifiState;

        isFirstDraw = false;
        currentEncoderValue = ossm->encoder.readEncoder();

        if (xSemaphoreTake(displayMutex, 100) == pdTRUE) {
            clearPage(true, false); // Clear page content but preserve header icons

            // Drawing Variables.
            int leftPadding = 6;  // Padding on the left side of the screen
            int fontSize = 8;
            int itemHeight = 20;   // Height of each item
            int visibleItems = 3;  // Number of items visible on the screen

            auto updateMenuOption = ossm->updateMenuOption;
            if (abs(currentEncoderValue % maxClicks -
                    lastEncoderValue % maxClicks) >= clicksPerRow) {
                lastEncoderValue = currentEncoderValue % maxClicks;
                updateMenuOption = (UpdateMenu)floor(lastEncoderValue / clicksPerRow);

                ossm->updateMenuOption = updateMenuOption;
            }

            ESP_LOGD(
                "Update Menu",
                "currentEncoderValue: %d, lastEncoderValue: %d, updateMenuOption: %d",
                currentEncoderValue, lastEncoderValue, updateMenuOption);

            drawShape::scroll(100 * ossm->encoder.readEncoder() /
                              (clicksPerRow * UpdateMenu::NUM_UPDATE_OPTIONS - 1));
            const char *menuName = updateMenuStrings[updateMenuOption];
            ESP_LOGD("Update Menu", "Hovering over state: %s", menuName);

            // Loop around to make an infinite menu.
            int lastIdx =
                updateMenuOption - 1 < UpdateMenu::HTTP_Update ? UpdateMenu::BackToCalibration : updateMenuOption - 1;
            int nextIdx =
                updateMenuOption + 1 > UpdateMenu::BackToCalibration ? UpdateMenu::HTTP_Update : updateMenuOption + 1;

            ossm->display.setFont(Config::Font::base);

            // Draw the previous item
            if (lastIdx >= UpdateMenu::HTTP_Update) {
                ossm->display.drawUTF8(leftPadding, itemHeight * (1),
                                       updateMenuStrings[lastIdx]);
            }

            // Draw the next item
            if (nextIdx <= UpdateMenu::BackToCalibration) {
                ossm->display.drawUTF8(leftPadding, itemHeight * (3),
                                       updateMenuStrings[nextIdx]);
            }

            // Draw the current item
            ossm->display.setFont(Config::Font::bold);
            ossm->display.drawUTF8(leftPadding, itemHeight * (2), menuName);

            // Draw a rounded rectangle around the center item
            ossm->display.drawRFrame(
                0,
                itemHeight * (visibleItems / 2) - (fontSize - itemHeight) / 2,
                120, itemHeight, 2);

            // Draw Shadow.
            ossm->display.drawLine(2, 2 + fontSize / 2 + 2 * itemHeight, 119,
                                   2 + fontSize / 2 + 2 * itemHeight);
            ossm->display.drawLine(120, 4 + fontSize / 2 + itemHeight, 120,
                                   1 + fontSize / 2 + 2 * itemHeight);

            refreshPage(true, true); // Include both footer and header in refresh
            xSemaphoreGive(displayMutex);
        }

        vTaskDelay(1);
    };

    // Clear header icons when exiting menu
    if (xSemaphoreTake(displayMutex, 100) == pdTRUE) {
        clearIcons(); // Clear the header icons
        ossm->display.setMaxClipWindow(); // Reset clipping
        ossm->display.sendBuffer(); // Send the cleared buffer to display
        xSemaphoreGive(displayMutex);
    }

    vTaskDelete(nullptr);
}

void OSSM::drawUpdateMenu() {
    int stackSize = 5 * configMINIMAL_STACK_SIZE;
    xTaskCreate(drawUpdateMenuTask, "drawUpdateMenuTask", stackSize, this, 1,
                &Tasks::drawUpdateMenuTaskH);
}

void OSSM::startCalibrationHoming() {
    int stackSize = 10 * configMINIMAL_STACK_SIZE;
    xTaskCreatePinnedToCore(startCalibrationHomingTask, "startCalibrationHomingTask", stackSize, this,
                            configMAX_PRIORITIES - 1, &Tasks::runCalibrationHomingTaskH,
                            Tasks::operationTaskCore);
}

void OSSM::startCalibrationHomingTask(void *pvParameters) {
    TickType_t xTaskStartTime = xTaskGetTickCount();

    // parse parameters to get OSSM reference
    OSSM *ossm = (OSSM *)pvParameters;

#ifdef AJ_DEVELOPMENT_HARDWARE
    ossm->stepper->setCurrentPosition(0);
    ossm->stepper->forceStopAndNewPosition(0);
    ossm->sm->process_event(CalibrationHomingDone{});
    vTaskDelete(nullptr);
    return;
#endif

    // Calibration homing treats this differently.
    ossm->stepper->enableOutputs();
    ossm->stepper->setDirectionPin(Pins::Driver::motorDirectionPin, false);
    int16_t sign = ossm->sm->is("calibration.calibrating.backward"_s) ? 1 : -1;

    int32_t targetPositionInSteps =
        round(sign * Config::Driver::maxStrokeSteps);

    ESP_LOGD("Calibration Homing", "Target position in steps: %d", targetPositionInSteps);
    ossm->stepper->moveTo(targetPositionInSteps, false);

    auto isInCorrectState = [](OSSM *ossm) {
        // Add any states that you want to support here.
        return ossm->sm->is("calibration.calibrating"_s) || ossm->sm->is("calibration.calibrating.forward"_s) ||
               ossm->sm->is("calibration.calibrating.backward"_s);
    };

    // run loop for 15second or until loop exits
    while (isInCorrectState(ossm)) {
        TickType_t xCurrentTickCount = xTaskGetTickCount();
        // Calculate the time in ticks that the task has been running.
        TickType_t xTicksPassed = xCurrentTickCount - xTaskStartTime;

        // If you need the time in milliseconds, convert ticks to milliseconds.
        // 'portTICK_PERIOD_MS' is the number of milliseconds per tick.
        uint32_t msPassed = xTicksPassed * portTICK_PERIOD_MS;

        if (msPassed > 40000) {
            ESP_LOGE("Calibration Homing", "Homing took too long. Check power and restart");
            ossm->errorMessage = UserConfig::language.HomingTookTooLong;

            // Clear homing active flag for LED indication
            setHomingActive(false);

            ossm->sm->process_event(Error{});
            break;
        }

        // measure the current analog value.
        float current = getAnalogAveragePercent(
                            SampleOnPin{Pins::Driver::currentSensorPin, 200}) -
                        ossm->currentSensorOffset;

        ESP_LOGV("Calibration Homing", "Current: %f", current);
        bool isCurrentOverLimit =
            current > Config::Driver::sensorlessCurrentLimit;

        if (!isCurrentOverLimit) {
            vTaskDelay(10); // Increased from 1ms to 10ms to reduce CPU load
            continue;
        }

        ESP_LOGD("Calibration Homing", "Current over limit: %f", current);
        ossm->stepper->stopMove();

        // step away from the hard stop, with your hands in the air!
        int32_t currentPosition = ossm->stepper->getCurrentPosition();
        ossm->stepper->moveTo(currentPosition - sign * Config::Driver::homingOffsetMn, true);

        // measure and save the current position
        ossm->measuredStrokeSteps =
            min(float(abs(ossm->stepper->getCurrentPosition())),
                Config::Driver::maxStrokeSteps);

        ossm->stepper->setCurrentPosition(0);
        ossm->stepper->forceStopAndNewPosition(0);

        // Clear homing active flag for LED indication
        setHomingActive(false);

        ossm->sm->process_event(CalibrationHomingDone{});
        break;
    };

    vTaskDelete(nullptr);
}

void OSSM::setCalibrated() {
    isCalibrated = true;
    // Save calibration data to NVS
    saveCalibrationData();
    ESP_LOGD("OSSM", "Calibration completed and saved to NVS");
}

bool OSSM::initNVS() {
    esp_err_t err = nvs_flash_init();
    if (err != ESP_OK) {
        // If initialization failed, try erasing and reinitializing
        ESP_LOGW("OSSM", "NVS initialization failed, attempting to erase and retry: %s", esp_err_to_name(err));
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
        if (err != ESP_OK) {
            ESP_LOGE("OSSM", "Failed to initialize NVS after erase: %s", esp_err_to_name(err));
            return false;
        }
    }

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvsHandle);
    if (err != ESP_OK) {
        ESP_LOGE("OSSM", "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return false;
    }

    ESP_LOGD("OSSM", "NVS initialized successfully");
    return true;
}

bool OSSM::saveCalibrationData() {
    if (nvsHandle == 0) {
        ESP_LOGE("OSSM", "NVS not initialized, cannot save calibration data");
        return false;
    }

    // Create calibration data struct
    CalibrationData data;
    data.measuredStrokeSteps = measuredStrokeSteps;
    data.isCalibrated = isCalibrated;

    // Save the struct as raw data
    esp_err_t err = nvs_set_blob(nvsHandle, NVS_KEY_CALIBRATION_DATA, &data, sizeof(CalibrationData));
    if (err != ESP_OK) {
        ESP_LOGE("OSSM", "Failed to save calibration data to NVS: %s", esp_err_to_name(err));
        return false;
    }

    err = nvs_commit(nvsHandle);
    if (err != ESP_OK) {
        ESP_LOGE("OSSM", "Failed to commit calibration data to NVS: %s", esp_err_to_name(err));
        return false;
    }

    ESP_LOGD("OSSM", "Saved calibration data to NVS: measuredStrokeSteps=%f, isCalibrated=%s",
             data.measuredStrokeSteps, data.isCalibrated ? "true" : "false");
    return true;
}

bool OSSM::loadCalibrationData() {
    if (nvsHandle == 0) {
        ESP_LOGE("OSSM", "NVS not initialized, cannot load calibration data");
        return false;
    }

    // Create calibration data struct
    CalibrationData data;
    size_t dataSize = sizeof(CalibrationData);

    // Load the struct from raw data
    esp_err_t err = nvs_get_blob(nvsHandle, NVS_KEY_CALIBRATION_DATA, &data, &dataSize);
    if (err != ESP_OK) {
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGD("OSSM", "No calibration data found in NVS, using default values");
            // Set default values
            measuredStrokeSteps = 0;
            isCalibrated = false;
        } else {
            ESP_LOGE("OSSM", "Failed to load calibration data from NVS: %s", esp_err_to_name(err));
        }
        return false;
    }

    // Verify data size
    if (dataSize != sizeof(CalibrationData)) {
        ESP_LOGE("OSSM", "Calibration data size mismatch: expected %d, got %d",
                 sizeof(CalibrationData), dataSize);
        return false;
    }

    // Apply loaded data
    measuredStrokeSteps = data.measuredStrokeSteps;
    isCalibrated = data.isCalibrated;

    ESP_LOGD("OSSM", "Loaded calibration data from NVS: measuredStrokeSteps=%f, isCalibrated=%s",
             measuredStrokeSteps, isCalibrated ? "true" : "false");
    return true;
}

void OSSM::cleanupNVS() {
    if (nvsHandle != 0) {
        nvs_close(nvsHandle);
        nvsHandle = 0;
        ESP_LOGD("OSSM", "NVS handle closed");
    }
}

void OSSM::drawInDevelopment() {
    if (xSemaphoreTake(displayMutex, 100) == pdTRUE) {
        clearPage(true, true);
        drawStr::title(UserConfig::language.InDevelopment);
        drawStr::multiLine(0, 20, "This feature is currently in development.");
        drawStr::multiLine(0, 35, "Please check back in future releases.");
        refreshPage(true, true);
        xSemaphoreGive(displayMutex);
    }
}

void OSSM::drawOTA() {
    if (xSemaphoreTake(displayMutex, 100) == pdTRUE) {
        clearPage(true, true);
        drawStr::title("OTA Update Setup");

        display.setFont(Config::Font::base);
        display.drawUTF8(0, 24, "Device Info:");
        display.drawUTF8(0, 36, ("Hostname: " + String(UserConfig::otaHostname)).c_str());
        display.drawUTF8(0, 48, ("IP: " + OTAService::getIPAddress()).c_str());
        display.drawUTF8(0, 60, ("Password: " + String(UserConfig::otaPassword)).c_str());

        refreshPage(true, true);
        xSemaphoreGive(displayMutex);
    }
}

void OSSM::drawOTAProgress() {
    if (xSemaphoreTake(displayMutex, 100) == pdTRUE) {
        clearPage(true, true);
        drawStr::title("OTA Update In Progress");

        display.setFont(Config::Font::base);
        display.drawUTF8(0, 24, "Please wait...");
        display.drawUTF8(0, 36, "Do not power off device");

        // Draw progress bar placeholder
        display.drawFrame(0, 50, 128, 10);
        display.drawBox(0, 50, 0, 10); // Empty progress bar

        refreshPage(true, true);
        xSemaphoreGive(displayMutex);
    }
}

void OSSM::drawOTAComplete() {
    if (xSemaphoreTake(displayMutex, 100) == pdTRUE) {
        clearPage(true, true);
        drawStr::title("OTA Update Complete");

        display.setFont(Config::Font::base);
        display.drawUTF8(0, 24, "Firmware updated");
        display.drawUTF8(0, 36, "successfully!");
        display.drawUTF8(0, 60, UserConfig::language.Restart);

        refreshPage(true, true);
        xSemaphoreGive(displayMutex);
    }
}

void OSSM::drawOTAFailed() {
    if (xSemaphoreTake(displayMutex, 100) == pdTRUE) {
        clearPage(true, true);
        drawStr::title("OTA Update Failed");

        display.setFont(Config::Font::base);
        display.drawUTF8(0, 24, "Update failed.");
        display.drawUTF8(0, 36, "Please try again.");
        display.drawUTF8(0, 60, "Click to retry");

        refreshPage(true, true);
        xSemaphoreGive(displayMutex);
    }
}

void OSSM::startOTA() {
    ESP_LOGI(UPDATE_TAG, "Starting OTA update");
    // OTA update will be handled by ArduinoOTA in the main loop
    // through OTAService::handle()
}
