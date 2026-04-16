#ifndef OSSM_SIMPLE_PENETRATION_H
#define OSSM_SIMPLE_PENETRATION_H

namespace simple_penetration {

    /**
     * Start the simple penetration motion task
     * Basic back-and-forth motion at controlled speed
     */
    void startSimplePenetration();

    /**
     * Pause simple penetration by stopping the stepper.
     * The background task remains alive; motion resumes when resumeSimplePenetration() is called.
     */
    void pauseSimplePenetration();

    /**
     * Resume simple penetration after a pause.
     */
    void resumeSimplePenetration();

    /**
     * Stop and return to home position.
     */
    void returnHome();

}  // namespace simple_penetration

#endif  // OSSM_SIMPLE_PENETRATION_H
