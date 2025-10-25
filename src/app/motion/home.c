#include "home.h"

#include "axis.h"
#include "delay.h"
#include "limits.h"
#include "motion_units.h"
#include "stepgen_pwm_tim3.h"

static inline void poll_1ms(void) {
    limits_poll_tick();
    delay(1);
}

// Convert direction intent to the stepgen's CW boolean
static inline void set_dir_toward(axis_t a, bool toward_negative) {
    bool cw = toward_negative ? axis_cw_is_negative(a) : !axis_cw_is_negative(a);
    stepgen_dir(a, cw);
}

/**
 * Move a distance in mm at feed (mm/min) in the given direction
 * BLocks until the motion completes or is stopped early by the MIN switch
 * (ISR already handles the early stop)
 */
static void move_mm_blocking(axis_t a, float mm, float feed_mm_min, bool toward_negative) {
    set_dir_toward(a, toward_negative);
    stepgen_move_n(a, mm_to_steps(a, mm), feed_to_hz(a, feed_mm_min));
    while (stepgen_busy(a)) {
        poll_1ms(); // keep the debouncer fresh during motion
    }
}

void home_init(void) {
    limits_init_min(); // seed debouncers from current pin level
    // Give the debouncer a few ms to settle
    for (int i = 0; i < 10; ++i) {
        poll_1ms();
    }
}

/* Back-off until MIN is released (safety), then do fast-seek, release, slow-seek,
   and final clearance to home_offset_mm. Leaves you un-pressed and homed. */
bool home_axis_blocking(axis_t a, const home_params_t* p) {
    // Ensure driver is enabled (TMC2209: low-active enable)
    stepgen_enable(a, true);

    // 0) If we start on the switch, back off first.
    if (limits_min_pressed(a)) {
        move_mm_blocking(a, p->backoff_mm, p->slow_feed_mm_min, /*toward_negative=*/false);
        // Wait until debounced release (should be immediate after backoff)
        for (int i = 0; i < 20 && limits_min_pressed(a); ++i)
            poll_1ms();
        if (limits_min_pressed(a))
            return false; // still stuck -> wiring/mechanics issue
    }

    // 1) FAST SEEK toward MIN; ISR will stop early on hit.
    move_mm_blocking(a, p->seek_span_mm, p->fast_feed_mm_min, /*toward_negative=*/true);
    if (!limits_min_pressed(a)) {
        // Never hit the switch within the span -> not found
        return false;
    }

    // 2) BACK OFF to clear the switch
    move_mm_blocking(a, p->backoff_mm, p->slow_feed_mm_min, /*toward_negative=*/false);
    for (int i = 0; i < 50 && limits_min_pressed(a); ++i)
        poll_1ms();
    if (limits_min_pressed(a))
        return false;

    // 3) SLOW SEEK to re-latch precisely
    move_mm_blocking(a, p->backoff_mm * 2.0f, p->slow_feed_mm_min, /*toward_negative=*/true);
    if (!limits_min_pressed(a))
        return false;

    // 4) Final clearance to home_offset (leave switch released)
    if (p->home_offset_mm >= 0.0f) {
        move_mm_blocking(a, p->home_offset_mm, p->slow_feed_mm_min, /*toward_negative=*/false);
        for (int i = 0; i < 50 && limits_min_pressed(a); ++i)
            poll_1ms();
        if (limits_min_pressed(a))
            return false;
    }

    // (Future step will set machine position = 0 here; for now we just report success.)
    return true;
}