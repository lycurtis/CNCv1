#include "app_init.h"
#include "home.h"
#include "motion_units.h"
#include "stepgen_pwm_tim3.h"

int main(void) {
    app_init();
    motion_init_defaults();
    home_init();

    // Example params — tweak per axis mechanics:
    const home_params_t HX = {
            .fast_feed_mm_min = 1800.0f,
            .slow_feed_mm_min = 300.0f,
            .backoff_mm = 3.0f,
            .seek_span_mm = 200.0f, // larger than worst-case distance to MIN
            .home_offset_mm = 1.0f // end 1 mm off the switch, released
    };

    stepgen_enable(AXIS_X, true);

    // Home X only (one feature at a time)
    bool ok = home_axis_blocking(AXIS_X, &HX);
    (void)ok; // (optional) route to a debug print LED/UART later

    // Now you can do a test move in + direction (away from MIN)
    stepgen_dir(AXIS_X, /*CW=*/false); // or compute via axis semantics
    stepgen_move_n(AXIS_X, mm_to_steps(AXIS_X, 20.0f), feed_to_hz(AXIS_X, 1200.0f));
    while (stepgen_busy(AXIS_X)) {
    }

    for (;;) { /* superloop */
    }
}
