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

    stepgen_enable(AXIS_X, true);
    stepgen_enable(AXIS_Y, true);
    stepgen_enable(AXIS_Z, true);

    stepgen_dir(AXIS_X, DIR_CW);
    stepgen_dir(AXIS_Y, DIR_CW);
    stepgen_dir(AXIS_Z, DIR_CW);

    // IMPORTANT: single shared step frequency (TIM3 ARR is shared; last set wins)
    uint32_t hz = feed_to_hz(AXIS_Y, 1500.0f); // pick one base; all axes will use this hz

    // Arm all three axes back-to-back (no waits in between) → they run together
    stepgen_move_n(AXIS_X, mm_to_steps(AXIS_X, 35.0f), hz);
    while (stepgen_busy(AXIS_X)) {
    }

    stepgen_move_n(AXIS_Y, mm_to_steps(AXIS_Y, 25.0f), hz);
    while (stepgen_busy(AXIS_Y)) {
    }

    stepgen_move_n(AXIS_Z, mm_to_steps(AXIS_Z, 35.0f), hz);
    while (stepgen_busy(AXIS_Z)) {
    }

    stepgen_dir(AXIS_X, DIR_CCW);
    stepgen_dir(AXIS_Y, DIR_CCW);
    stepgen_dir(AXIS_Z, DIR_CCW);

    stepgen_move_n(AXIS_X, mm_to_steps(AXIS_X, 35.0f), hz);
    stepgen_move_n(AXIS_Z, mm_to_steps(AXIS_Z, 35.0f), hz);
    while (stepgen_busy(AXIS_X) || stepgen_busy(AXIS_Z)) {
    }

    stepgen_dir(AXIS_Z, DIR_CW);

    stepgen_move_n(AXIS_Y, mm_to_steps(AXIS_Y, 25.0f), hz);
    stepgen_move_n(AXIS_Z, mm_to_steps(AXIS_Z, 40.0f), hz);
    while (stepgen_busy(AXIS_Y) || stepgen_busy(AXIS_Z)) {
    }

    stepgen_dir(AXIS_X, DIR_CW);
    stepgen_dir(AXIS_Y, DIR_CW);
    stepgen_dir(AXIS_Z, DIR_CCW);

    stepgen_move_n(AXIS_X, mm_to_steps(AXIS_X, 40.0f), hz);
    stepgen_move_n(AXIS_Y, mm_to_steps(AXIS_Y, 35.0f), hz);
    stepgen_move_n(AXIS_Z, mm_to_steps(AXIS_Z, 60.0f), hz);
    while (stepgen_busy(AXIS_X) || stepgen_busy(AXIS_Y) || stepgen_busy(AXIS_Z)) {
    }

    stepgen_dir(AXIS_X, DIR_CCW);
    stepgen_dir(AXIS_Y, DIR_CCW);
    stepgen_dir(AXIS_Z, DIR_CCW);

    hz = feed_to_hz(AXIS_Y, 1000.0f); // pick one base; all axes will use this hz

    stepgen_move_n(AXIS_Y, mm_to_steps(AXIS_Y, 35.0f), hz);
    stepgen_move_n(AXIS_Z, mm_to_steps(AXIS_Z, 30.0f), hz);
    while (stepgen_busy(AXIS_Y) || stepgen_busy(AXIS_Z)) {
    }

    stepgen_dir(AXIS_Y, DIR_CW);
    stepgen_move_n(AXIS_X, mm_to_steps(AXIS_X, 40.0f), hz);
    stepgen_move_n(AXIS_Y, mm_to_steps(AXIS_X, 40.0f), hz);
    while (stepgen_busy(AXIS_X) | stepgen_busy(AXIS_Y)) {
    }

    stepgen_dir(AXIS_Y, DIR_CCW);
    stepgen_dir(AXIS_X, DIR_CW);

    stepgen_move_n(AXIS_X, mm_to_steps(AXIS_X, 20.0f), hz);
    stepgen_move_n(AXIS_Y, mm_to_steps(AXIS_Y, 15.0f), hz);
    stepgen_move_n(AXIS_Z, mm_to_steps(AXIS_Z, 10.0f), hz);
    while (stepgen_busy(AXIS_X) || stepgen_busy(AXIS_Y) || stepgen_busy(AXIS_Z)) {
    }

    for (;;) { /* superloop */
    }
}
