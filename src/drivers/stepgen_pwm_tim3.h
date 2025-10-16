#pragma once

#include <stdbool.h>
#include <stdint.h>


/*
On STM32F446 with SYSCLK=180MHz, APB1 prescaler = 4
==> APB1 bus clock = 180/4 = 45MHz
Timers on STM32 have special rules:
-Case 1: If APB prescaler = 1 --> timer clock = APB clock
-Case 2: If APB precaaler > 1 --> timer clock = 2*APB clock
==> TIM3clk=2*APB1=90MHz.
Now the counter clock frequency (CK_CNT) = fck_psc/(PSC[15:0]+1)
we want CK_CNT = 1MHz = 90MHz(PSC + 1)
Therefore PSC = (90MHz/1MHz) - 1 = 89  TIM_PSC_1MHz (90UL - 1UL)
*/

typedef enum { AXIS_X = 0, AXIS_Y = 1, AXIS_Z = 2 } axis_t;

void stepgen_init_all(void);
void stepgen_start_all(void);
void stepgen_stop_all(void);

void stepgen_enable(axis_t a, bool enable_outputs_low_active);
void stepgen_dir(axis_t a, bool fwd);
void stepgen_set_hz(axis_t a, uint32_t hz);
