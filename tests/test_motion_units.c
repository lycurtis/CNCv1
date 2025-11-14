#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "motion_units.h"


static void test_defaults_steps_per_mm(void) {
    motion_init_defaults();

    float x_spmm = steps_per_mm(AXIS_X);
    float y_spmm = steps_per_mm(AXIS_Y);
    float z_spmm = steps_per_mm(AXIS_Z);

    // Allow tiny float tolerance
    assert(fabsf(x_spmm - 200.0f) < 0.001f);
    assert(fabsf(y_spmm - 200.0f) < 0.001f);
    assert(fabsf(z_spmm - 200.0f) < 0.001f);
}

static void test_mm_to_steps_rounding(void) {
    motion_init_defaults();

    // Exact multiple
    assert(mm_to_steps(AXIS_X, 10.0f) == 2000u);

    // Check rounding behavior
    // 0.01 mm * 200 steps/mm = 2 steps ⇒ expect 2
    assert(mm_to_steps(AXIS_X, 0.01f) == 2u);

    // 0.002 mm * 200 = 0.4 steps ⇒ with +0.5f expect 0 or 1 depending on your policy
    // Right now: (0.4 + 0.5) = 0.9 → cast to uint32_t = 0
    assert(mm_to_steps(AXIS_X, 0.002f) == 0u);
}

static void test_feed_to_hz_basic(void) {
    motion_init_defaults();

    // 600 mm/min = 10 mm/s; 10 mm/s * 200 steps/mm = 2000 Hz
    assert(feed_to_hz(AXIS_X, 600.0f) == 2000u);

    // 0 feed should give 0 Hz
    assert(feed_to_hz(AXIS_X, 0.0f) == 0u);
}

int main(void) {
    test_defaults_steps_per_mm();
    test_mm_to_steps_rounding();
    test_feed_to_hz_basic();

    printf("All motion_units tests passed.\n");
    return 0;
}