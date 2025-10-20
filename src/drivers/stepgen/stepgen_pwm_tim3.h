#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "axis.h"

void stepgen_init_all(void);
void stepgen_start_all(void);
void stepgen_stop_all(void);

void stepgen_enable(axis_t a, bool enable_outputs_low_active);
void stepgen_dir(axis_t a, bool fwd);
void stepgen_set_hz(axis_t a, uint32_t hz);

void stepgen_move_n(axis_t a, uint32_t steps, uint32_t hz);
bool stepgen_busy(axis_t a); // quick poll to know if a move is still running on that axis
