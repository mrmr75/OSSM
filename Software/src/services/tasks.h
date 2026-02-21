#ifndef OSSM_SOFTWARE_TASKS_H
#define OSSM_SOFTWARE_TASKS_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace Tasks {
    // Declare variables as extern
    extern TaskHandle_t drawHelloTaskH;
    extern TaskHandle_t drawMenuTaskH;
    extern TaskHandle_t drawPlayControlsTaskH;
    extern TaskHandle_t drawPatternControlsTaskH;
    extern TaskHandle_t wmTaskH;
    extern TaskHandle_t drawPreflightTaskH;

    extern TaskHandle_t runHomingTaskH;
    extern TaskHandle_t runSimplePenetrationTaskH;
    extern TaskHandle_t runStrokeEngineTaskH;

    extern volatile TaskHandle_t activeUiTaskH;
    extern volatile TaskHandle_t activeBackgroundTaskH;


    // Constants can stay in the header
    constexpr int stepperCore = 1;
    constexpr int operationTaskCore = 0;

    void startUiTask(TaskFunction_t uiWorkerTask, const char* taskName, void* params);
    void startBackgroundTask(TaskFunction_t uiWorkerTask, const char* taskName, void* params);
}

#endif  // OSSM_SOFTWARE_TASKS_H
