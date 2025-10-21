#include "app_init.h"

#include "bsp_usart2_debug.h"
#include "estop.h"
#include "limits.h"
#include "motion_units.h"
#include "stepgen_pwm_tim3.h"
#include "stm32f4xx.h"
#include "system_clock.h"


static inline void debounce_tick_1k(void) {
    limits_poll_tick();
    estop_poll_tick();
}

void SysTick_Handler(void) {
    debounce_tick_1k();
}

// Keep device headers out of app layer on purpose.
// (Only BSP/drivers should include <stm32f4xx.h>)

void app_init(void) {
    system_clock_init(); // SoC clocks
    SysTick_Config(SystemCoreClock / 1000U);
    const uint32_t pclk1 = 45000000UL; // APB1 after clock setup
    dbg_uart_init(pclk1, 115200); // early logging

    // Board GPIO is inited lazily by each driver/bsp module as needed.
    estop_init(); // emergency braking system
    limits_init_min(); // limit switch
    stepgen_init_all(); // timer + pins for stepper STEP
    motion_init_defaults(); // steps/mm config
}
