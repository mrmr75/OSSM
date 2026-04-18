#ifndef SOFTWARE_REFERENCEPOSITIONS_H
#define SOFTWARE_REFERENCEPOSITIONS_H
#include <optional>
struct ReferencePositions {
    // Manual calibration points (stored as absolute steps)
    // These are session-only and not persisted to NVS
    std::optional<long> justOutPosition = std::nullopt;
    std::optional<long> justInPosition = std::nullopt;
    std::optional<long> maxDepthPosition = std::nullopt;
    std::optional<long> referencePosition = std::nullopt;
};
#endif  // SOFTWARE_REFERENCEPOSITIONS_H
