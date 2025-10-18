#include "app_init.h"
#include "motion_units.h"
#include "stepgen_pwm_tim3.h"

#define DIR_CW true
#define DIR_CCW false

int main(void) {
    app_init();

    stepgen_enable(AXIS_X, true);
    stepgen_dir(AXIS_X, DIR_CW);
    stepgen_move_n(AXIS_X,
                   mm_to_steps(AXIS_X, 50.0f),
                   feed_to_hz(AXIS_X, 2200.0f)); // Example: +50 mm @ 2200 mm/min
    while (stepgen_busy(AXIS_X)) {
    }

    for (;;) { /* superloop */
    }
}
