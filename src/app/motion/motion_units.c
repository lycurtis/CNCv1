#include "motion_units.h"
static axis_cfg_t cfg[3];

void motion_init_defaults(void) {
    cfg[AXIS_X] = (axis_cfg_t){200, 8, 8.0f};
    cfg[AXIS_Y] = (axis_cfg_t){200, 8, 8.0f};
    cfg[AXIS_Z] = (axis_cfg_t){200, 8, 8.0f};
}

float steps_per_mm(axis_t a) {
    float spr = (float)cfg[a].full_steps_rev * (float)cfg[a].microsteps; // steps per revolution
    return spr / cfg[a].mm_per_rev;
}

uint32_t mm_to_steps(axis_t a, float mm) {
    return (uint32_t)(steps_per_mm(a) * mm + 0.5f);
}

uint32_t feed_to_hz(axis_t a, float feed_mm_min) {
    float feed_mm_s = feed_mm_min / 60.0f;
    return (uint32_t)(steps_per_mm(a) * feed_mm_s + 0.5f);
}
