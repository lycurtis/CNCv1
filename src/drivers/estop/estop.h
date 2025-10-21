#pragma once

#include <stdbool.h>

void estop_init(void);
bool estop_latched(void);
void estop_clear(void);

void estop_poll_tick(void); // call at 1 kHz for debounce + latch