#include "app_init.h"
#include "home.h"
#include "motion_units.h"
#include "stepgen_pwm_tim3.h"

static bool home_one(axis_t a, float fast, float slow, float backoff, float span, float offset) {
    stepgen_enable(a, true); // ensure driver enabled
    const home_params_t p = {.fast_feed_mm_min = fast,
                             .slow_feed_mm_min = slow,
                             .backoff_mm = backoff,
                             .seek_span_mm = span,
                             .home_offset_mm = offset};
    return home_axis_blocking(a, &p);
}

static void test_moves_after_home(void) {
    // Simple sanity moves away from MIN after homing:
    const float test_mm = 20.0f;
    const float feed = 1200.0f;

    // X +20
    stepgen_dir(AXIS_X, /*CW=*/false); // if CW is your + direction for X
    stepgen_move_n(AXIS_X, mm_to_steps(AXIS_X, test_mm), feed_to_hz(AXIS_X, feed));
    while (stepgen_busy(AXIS_X)) {
    }

    // Y +20
    stepgen_dir(AXIS_Y, /*CW=*/false);
    stepgen_move_n(AXIS_Y, mm_to_steps(AXIS_Y, test_mm), feed_to_hz(AXIS_Y, feed));
    while (stepgen_busy(AXIS_Y)) {
    }

    // (Z optional—see safety note below)
    stepgen_dir(AXIS_Z, /*CW=*/true);
    stepgen_move_n(AXIS_Z, mm_to_steps(AXIS_Z, test_mm), feed_to_hz(AXIS_Z, feed));
    while (stepgen_busy(AXIS_Z)) {
    }
}

int main(void) {
    app_init();
    motion_init_defaults();
    home_init();

    // TUNE per axis mechanics. Start conservative:
    const float FAST = 1800.0f; // fast seek
    const float SLOW = 300.0f; // slow latch
    const float BACK = 3.5f; // mm to clear switch
    const float SPAN = 220.0f; // long enough to *guarantee* you reach MIN
    const float OFFS = 1.0f; // sit 1 mm off the switch at the end

    bool okX = home_one(AXIS_X, FAST, SLOW, BACK, SPAN, OFFS);
    bool okY = home_one(AXIS_Y, FAST, SLOW, BACK, SPAN, OFFS);
    bool okZ = home_one(AXIS_Z, FAST, SLOW, BACK, SPAN, OFFS);

    // Simple success check (replace with LEDs/UART if you have them):
    volatile bool all_ok = okX && okY && okZ;

    // Try some gentle + moves away from MIN to verify directions/clearance
    if (all_ok) {
        test_moves_after_home();
    }

    for (;;) { /* superloop */
    }
}
