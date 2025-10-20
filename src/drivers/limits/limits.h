#pragma once

#include <stdbool.h>

#include "stepgen_pwm_tim3.h" // for axis_t; later we can move axis_t to config/axis.h

void limits_init_min(void); // configure X/Y/Z MIN pins as input + pull-up
bool limits_min_pressed(axis_t a); // raw reading with polarity from bsp_pins.h
bool limits_block_neg(axis_t a); // true if we must block motion toward MIN