#pragma once

#include "stm32f4xx.h"
#include <stdint.h>
#include <stdbool.h>

#include "bsp_pins.h"
#include "bsp_gpio.h"
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
#define TIM_PSC_1MHz (90UL - 1UL) 
 
// X Axis
// #define X_STEP_PORT GPIOA
// #define X_STEP_PIN 6UL // PA6 (TIM3_CH1) AF2
#define X_STEP_AF_VAL 2UL // AF2 -> TIM3_CH1
// #define X_DIR_PORT GPIOB
// #define X_DIR_PIN 0UL // PB0
// #define X_EN_PORT GPIOB
// #define X_EN_PIN 1UL // PB1 (note: LOW = enable)

void stepgen_init();
void stepgen_start(void);
void stepgen_stop(void);
void stepgen_set_hz(uint32_t hz);
void stepgen_dir(bool fwd);
void stepgen_enable(bool enable_outputs_low_active);