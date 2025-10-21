#pragma once

#include <stdbool.h>

#include "axis.h"

void limits_init_min(void); // configure X/Y/Z MIN pins as input + pull-up
void limits_poll_tick(void);
bool limits_min_pressed(axis_t a); // raw reading with polarity from bsp_pins.h
bool limits_block_neg(axis_t a); // true if we must block motion toward MIN

void limits_poll_tick(void);