#include "tasks.h"

namespace Tasks {
    TaskHandle_t drawHelloTaskH = nullptr;
    TaskHandle_t drawMenuTaskH = nullptr;
    TaskHandle_t drawPlayControlsTaskH = nullptr;
    TaskHandle_t drawPatternControlsTaskH = nullptr;
    TaskHandle_t wmTaskH = nullptr;
    TaskHandle_t drawPreflightTaskH = nullptr;

    TaskHandle_t runHomingTaskH = nullptr;
    TaskHandle_t runSimplePenetrationTaskH = nullptr;
    TaskHandle_t runStrokeEngineTaskH = nullptr;

    volatile TaskHandle_t activeUiTaskH = nullptr;
    volatile TaskHandle_t activeBackgroundTaskH = nullptr;

    // Generic template function to handle task management with identical logic
    template<typename TaskHandlePtr>
    void startNewTask(TaskFunction_t workerTask, const char* taskName, void* params, volatile TaskHandlePtr& activeTaskHandle) {
        if (activeTaskHandle != NULL) {
            // 1. Signal the old task to wrap up
            xTaskNotifyGive(activeTaskHandle);

            // 2. The Loop: Wait for the task to "Check In" as dead
            // We use a timeout so we don't hang the S3 forever if the task is stuck
            int timeout = 0;
            while (activeTaskHandle != NULL && timeout < 100) {
                vTaskDelay(pdMS_TO_TICKS(10));
                timeout++;
            }

            if (timeout >= 100) {
                // Serial.println("Warning: Old task is a zombie. Force deleting.");
                vTaskDelete(activeTaskHandle);
                activeTaskHandle = NULL;
            }
        }

        // Now it's safe to start the next one
        xTaskCreate(workerTask, taskName, 4096, params, 1, (TaskHandle_t*)&activeTaskHandle);
    }

    void startUiTask(TaskFunction_t uiWorkerTask, const char* taskName, void* params) {
        startNewTask(uiWorkerTask, taskName, params, activeUiTaskH);
    }

    void startBackgroundTask(TaskFunction_t uiWorkerTask, const char* taskName, void* params) {
        startNewTask(uiWorkerTask, taskName, params, activeBackgroundTaskH);
    }
}
