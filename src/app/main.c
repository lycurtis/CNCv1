#include "main.h"
#include "stm32f4xx.h"


int main(void){
    system_clock_init(); // 180 MHz PLL setup
    uint32_t pclk1 = 45000000UL; // APB1
    // uint32_t pclk2 = 90000000UL; // APB2
    dbg_uart_init(pclk1, 115200);

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
        dbg_write("Hello\r\n");
    }
}
