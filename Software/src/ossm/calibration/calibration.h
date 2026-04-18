#include <optional>

namespace calibrations {
    float justOutPositionPct();
    float justInPositionPct();
    float maxDepthPositionPct();
    float referencePositionPct();
    long percentageToSteps(float percentage);
    float stepsToPercentage(long steps);
    void drawCalibrationControls();
    void endCalibrationMode();
};
