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

int main(void) {
    app_init();
    motion_init_defaults();
    home_init();

    // TUNE per axis mechanics. Start conservative:
    const float FAST = 1800.0f; // fast seek
    const float SLOW = 300.0f; // slow latch
    const float BACK = 3.0f; // mm to clear switch
    const float SPAN = 220.0f; // long enough to *guarantee* you reach MIN
    const float OFFS = 1.0f; // sit 1 mm off the switch at the end

    bool okX = home_one(AXIS_X, FAST, SLOW, BACK, SPAN, OFFS);
    bool okY = home_one(AXIS_Y, FAST, SLOW, BACK, SPAN, OFFS);
    bool okZ = home_one(AXIS_Z, FAST, SLOW, BACK, SPAN, OFFS);

    // Simple success check (replace with LEDs/UART if you have them):
    volatile bool all_ok = okX && okY && okZ;

    for (;;) { /* superloop */
    }
}
