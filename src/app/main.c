#include "app_init.h"
#include "motion_units.h"
#include "stepgen_pwm_tim3.h"


#define DIR_CW true
#define DIR_CCW false

int main(void) {
    app_init();

    // Example: +10 mm @ 1200 mm/min
    stepgen_enable(AXIS_X, true);
    stepgen_dir(AXIS_X, DIR_CW);
    stepgen_move_n(AXIS_X, mm_to_steps(AXIS_X, 10.0f), feed_to_hz(AXIS_X, 1200.0f));
    while (stepgen_busy(AXIS_X)) {
    }

    for (;;) { /* superloop */
    }
}
