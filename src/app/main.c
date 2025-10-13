#include "main.h"

volatile uint32_t pclk1;
volatile uint32_t brrg;

int main(void){
    system_clock_init(); // 180 MHz PLL setup
    pclk1 = System_GetPCLK1();
    brrg = 115200;
    dbg_uart_init(45000000, 115200);

    stepgen_init_all();

    stepgen_enable(AXIS_X, true);
    stepgen_enable(AXIS_Y, true);
    stepgen_enable(AXIS_Z, true);

    stepgen_dir(AXIS_X, true);
    stepgen_dir(AXIS_Y, false);
    stepgen_dir(AXIS_Z, true);

    // Vary speed here
    /* Shared freq for now (TIM3): 1 kHz */
    stepgen_set_hz(AXIS_X, 1000);
    stepgen_set_hz(AXIS_Y, 1000);
    stepgen_set_hz(AXIS_Z, 1000);

    stepgen_start_all();

    while(1){
        dbg_putc('Y');
    }
}
