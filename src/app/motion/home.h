#pragma once
#include <stdbool.h>

#include "axis.h"

typedef struct {
    float fast_feed_mm_min; // fast seek speed
    float slow_feed_mm_min; // slow latch pass
    float backoff_mm; // how far to back off after a hit
    float seek_span_mm;
    float home_offset_mm; // where to leave the axis after homing (>=0, usually a tiny clearance)
} home_params_t;

void home_init(void);

// returns true on success (switch found and latched), false if not found
bool home_axis_blocking(axis_t a, const home_params_t* p);