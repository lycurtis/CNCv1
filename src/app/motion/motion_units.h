#pragma once
#include <stdint.h>

#include "stepgen_pwm_tim3.h" // for axis_t

typedef struct {
    uint16_t full_steps_rev; // e.g., 200
    uint16_t microsteps; // e.g., 8 --> 200*8 - 1600 steps/rev
    float mm_per_rev; // e.g., 8.0 for TR8x8
} axis_cfg_t;

void motion_init_defaults(void);
float steps_per_mm(axis_t a);
uint32_t mm_to_steps(axis_t a, float mm);
uint32_t feed_to_hz(axis_t a, float feed_mm_min); // feed in mm/min → steps/s
