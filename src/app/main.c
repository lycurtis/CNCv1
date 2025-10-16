#include "bsp_usart2_debug.h"
#include "delay.h"
#include "stepgen_pwm_tim3.h"
#include "stm32f4xx.h" // CMSIS device header
#include "system_clock.h"

#define DIR_CW true
#define DIR_CCW false

int main(void) {
    system_clock_init(); // 180 MHz PLL setup
    uint32_t pclk1 = 45000000UL; // APB1
    // uint32_t pclk2 = 90000000UL; // APB2
    dbg_uart_init(pclk1, 115200);

    stepgen_init_all();

    stepgen_enable(AXIS_X, true);
    // stepgen_enable(AXIS_Y, true);
    // stepgen_enable(AXIS_Z, true);

    stepgen_dir(AXIS_X, DIR_CW);
    // stepgen_dir(AXIS_Y, DIR_CCW);
    // stepgen_dir(AXIS_Z, DIR_CW);

    // Vary speed here
    /* Shared freq for now (TIM3): 1 kHz */
    // stepgen_set_hz(AXIS_X, 1000);
    // stepgen_set_hz(AXIS_Y, 1000);
    // stepgen_set_hz(AXIS_Z, 1000);

    // 2) Move 200 steps at 1000 steps/s (200 ms move)
    stepgen_move_n(AXIS_X, 200, 1000);

    // 3) Optionally poll until done (quick-and-dirty for now)
    while (stepgen_busy(AXIS_X)) { /* spin or later sleep */
    }

    // 4) Do another move the other way
    stepgen_dir(AXIS_X, DIR_CCW);
    stepgen_move_n(AXIS_X, 400, 800);
    while (stepgen_busy(AXIS_X)) {
    }

    // stepgen_start_all();

    while (1) {
    }
}
