#pragma once

#include <stdbool.h>

void estop_init(void);
bool estop_latched(void);
void estop_clear(void);

// Fast-path helpers (no debounce, safe to call from ISRs)
bool estop_pressed_raw(void);
void estop_latch_if_pressed_fast(void);