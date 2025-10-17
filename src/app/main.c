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

    // Y axis, single-axis verification
    stepgen_enable(AXIS_Y, true);
    stepgen_dir(AXIS_Y, DIR_CW);
    stepgen_move_n(AXIS_Y, 1600, 1000);
    while (stepgen_busy(AXIS_Y)) {
    }
    stepgen_dir(AXIS_Y, DIR_CCW);
    stepgen_move_n(AXIS_Y, 3200, 800);
    while (stepgen_busy(AXIS_Y)) {
    }

    // Z axis, single-axis verification
    stepgen_enable(AXIS_Z, true);
    stepgen_dir(AXIS_Z, DIR_CW);
    stepgen_move_n(AXIS_Z, 800, 500);
    while (stepgen_busy(AXIS_Z)) {
    }
    stepgen_dir(AXIS_Z, DIR_CCW);
    stepgen_move_n(AXIS_Z, 800, 500);
    while (stepgen_busy(AXIS_Z)) {
    }

    // Simple 2-axis concurrent move at SAME rate (shared TIM3)
    stepgen_enable(AXIS_X, true);
    stepgen_enable(AXIS_Y, true);
    stepgen_dir(AXIS_X, DIR_CW);
    stepgen_dir(AXIS_Y, DIR_CW);
    stepgen_move_n(AXIS_X, 1600, 1200); // sets shared timer to 1200 Hz
    stepgen_move_n(AXIS_Y, 1600, 1200); // same rate; both will decrement each update
    while (stepgen_busy(AXIS_X) || stepgen_busy(AXIS_Y)) {
    }

    while (1) {
    }
}
