#include "bsp_usart2_debug.h"
#include "delay.h"
#include "motion_units.h"
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

    motion_init_defaults();

    stepgen_enable(AXIS_X, true);
    stepgen_dir(AXIS_X, DIR_CW);

    stepgen_move_n(AXIS_X, mm_to_steps(AXIS_X, 10.0f), feed_to_hz(AXIS_X, 1200.0f));
    while (stepgen_busy(AXIS_X)) {
    }

    while (1) {
    } // super loop
}
