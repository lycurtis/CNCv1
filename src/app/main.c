#include "app_init.h"
#include "motion_units.h"
#include "stepgen_pwm_tim3.h"

#define DIR_CW true
#define DIR_CCW false

/**
 * X-Axis: CW = negative direction; CCW = positive direction
 * Y-Axis: CW = negative direction; CCW = positive direction
 * Z-Axis: CW = positive drection; CCW = negative direction
 */

int main(void) {
    app_init();

    // stepgen_enable(AXIS_X, true);
    // stepgen_enable(AXIS_Y, true);
    stepgen_enable(AXIS_Z, true);

    // stepgen_dir(AXIS_X, DIR_CW);
    // stepgen_dir(AXIS_Y, DIR_CCW);
    stepgen_dir(AXIS_Z, DIR_CCW);

    // IMPORTANT: single shared step frequency (TIM3 ARR is shared; last set wins)
    uint32_t hz = feed_to_hz(AXIS_Z, 1200.0f); // pick one base; all axes will use this hz

    // Arm all three axes back-to-back (no waits in between) → they run together
    stepgen_move_n(AXIS_Z, mm_to_steps(AXIS_Z, 20.0f), hz);
    while (stepgen_busy(AXIS_Z)) {
    }

    for (;;) { /* superloop */
    }
}
