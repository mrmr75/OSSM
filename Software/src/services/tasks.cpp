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
    volatile TaskHandle_t activeStreamingTaskH = nullptr;
    volatile TaskHandle_t activePublishTaskH = nullptr;


        // Template helper to cancel a task
    template<typename TaskHandlePtr>
    void cancelTask(volatile TaskHandlePtr& activeTaskHandle) {
        if (activeTaskHandle != NULL) {
            // Signal the task to wrap up
            xTaskNotifyGive(activeTaskHandle);

            // Wait for the task to clean itself up
            int timeout = 0;
            while (activeTaskHandle != NULL) {
                vTaskDelay(pdMS_TO_TICKS(10));
                timeout++;
            }

        }
    }

    // Generic template function to handle task management with identical logic
    template<typename TaskHandlePtr>
    void startNewTask(TaskFunction_t workerTask, const char* taskName, void* params, volatile TaskHandlePtr& activeTaskHandle) {
        cancelTask(activeTaskHandle);
        xTaskCreate(workerTask, taskName, 5 * configMINIMAL_STACK_SIZE, params, 1, (TaskHandle_t*)&activeTaskHandle);
    }

    void startUiTask(TaskFunction_t uiWorkerTask, const char* taskName, void* params) {
        startNewTask(uiWorkerTask, taskName, params, activeUiTaskH);
    }

    void startBackgroundTask(TaskFunction_t uiWorkerTask, const char* taskName, void* params) {
        startNewTask(uiWorkerTask, taskName, params, activeBackgroundTaskH);
    }

    void startStreamingTask(TaskFunction_t uiWorkerTask, const char* taskName, void* params) {
        startNewTask(uiWorkerTask, taskName, params, activeStreamingTaskH);
    }

    void startPublishingTask(TaskFunction_t uiWorkerTask, const char* taskName, void* params) {
        startNewTask(uiWorkerTask, taskName, params, activePublishTaskH);
    }

    void cancelActivePublishingTask() {
        cancelTask(activePublishTaskH);
    }

    void cancelActiveUiTask() {
        cancelTask(activeUiTaskH);
    }

    void cancelActiveBackgroundTask() {
        cancelTask(activeBackgroundTaskH);
    }

    void cancelActiveStreamingTask() {
        cancelTask(activeStreamingTaskH);
    }
}
